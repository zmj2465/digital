#include "data_recv_thread.h"

/*
功能：数据接收线程
参数：无
返回值：无
*/
void* data_recv_thread(void* arg)
{
    pthread_detach(pthread_self());
	int i;
	for (i = 0; i < info.simulated_link_num; i++)
	{
		printf("%d %d\n", i, FD[i].fd);
	}

    char data[MAX_DATA_LEN];
    int len;

    while (1)
    {
        /*接收消息并送往主控模块*/
		data_recv_proc();
    }
}


/*
功能：数据接收的处理，存入对应socket端口的接收数据缓存区
参数：无
返回值：0表示成功
*/
int data_recv_proc(void)
{
	int ret = 0;
	int maxfd = 0;
	msg_t msg;
	FD_ZERO(&RSET);
	int i = 0;
	for (i = 0; i < info.simulated_link_num; i++)
	{
		maxfd = FD[i].fd > maxfd ? FD[i].fd : maxfd;
		FD_SET(FD[i].fd, &RSET);
	}


	ret = select(maxfd + 1, &RSET, NULL, NULL, NULL);
	if (ret < 0)
	{
		ret = -1;
		printf("error\n");
		return ret;
	}

	for (i = 0; i < info.simulated_link_num && ret>0; i++)
	{
		if (FD_ISSET(FD[i].fd, &RSET))
		{
			ret--;
			if (FD[i].fd == LFD) {}
			else
			{
				ret = recv(FD[i].fd, FD[i].recvBuffer, sizeof(psy_msg_t), 0);
				if (ret > 0)
				{
					/*状态机判断*/
					if (fsm_status == FSM_INIT || fsm_status == FSM_OFF)
					{
						msg_t* rmsg = FD[i].recvBuffer;
						enqueue(&info.thread_queue[MASTER_THREAD_DATA], rmsg, MAX_DATA_LEN);
					}
					else
					{
						if (MY_INDEX != 0)
						{
							psy_msg_t* psy_msg = FD[i].recvBuffer;
							int len;
							int antenna_recv;
							antenna_recv = inquire_antenna(info.current_slot);
							/*信道仿真,匹配自身波束信息对齐*/
							ret = psy_recv(len, psy_msg, &msg, antenna_recv, MY_INDEX);
							if (ret == 0)
							{
								enqueue(&info.thread_queue[MASTER_THREAD_DATA], &msg, MAX_DATA_LEN);
							}
							else
							{
								continue;
							}

						}

					}
					

			
				}
			}
		}
	}

	return ret;
}