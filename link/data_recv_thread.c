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
				ret = recv(FD[i].fd, FD[i].recvBuffer, MAX_DATA_LEN, 0);
				if (ret > 0)
				{
					msg_t* msg = FD[i].recvBuffer;
					
					printf("base time=%lld,%ld start time=%d\n", msg->head.send_time.tv_sec, msg->head.send_time.tv_nsec, msg->data[1]);
					
					/*解包*/

					/*信道仿真*/

					/*仿真通过，打上实时时间戳，延时相应的时间并送往master线程的数据队列*/
					enqueue(&info.thread_queue[MASTER_THREAD_DATA], msg, 100);

					/*仿真失败则记录数据*/

				}
			}
		}
	}

	return ret;
}