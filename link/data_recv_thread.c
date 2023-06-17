#include "data_recv_thread.h"

/*
���ܣ����ݽ����߳�
��������
����ֵ����
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
        /*������Ϣ����������ģ��*/
		data_recv_proc();
    }
}


/*
���ܣ����ݽ��յĴ��������Ӧsocket�˿ڵĽ������ݻ�����
��������
����ֵ��0��ʾ�ɹ�
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
				ret = recv(FD[i].fd, FD[i].recvBuffer, sizeof(psy_msg_t), 0);
				if (ret > 0)
				{
					/*״̬���ж�*/
					if (fsm_status == FSM_INIT || fsm_status == FSM_OFF)
					{
						psy_msg_t* psy_msg = FD[i].recvBuffer;
						msg_t* rmsg = FD[i].recvBuffer;
						printf("no sim get, msg = %d psy_msg = %d msg_type = %d psy_type = %d\n", rmsg->head.dst, psy_msg->msg.head.dst, rmsg->head.type, psy_msg->msg.head.type);
						if (rmsg->head.type == START_GUN)
						{
							enqueue(&info.thread_queue[MASTER_THREAD_DATA], rmsg, MAX_DATA_LEN);
						}
					}
					else
					{
						psy_msg_t* psy_msg = FD[i].recvBuffer;
						msg_t msg;
						memset(&msg, 0, sizeof(msg_t));
						int len;
						int antenna_recv;
						antenna_recv = inquire_antenna(info.current_slot);
						/*�ŵ�����,ƥ����������Ϣ����*/
						ret = psy_recv(len, psy_msg, &msg, antenna_recv, info.device_info.node_role);
						if (ret == 0)
						{
							enqueue(&info.thread_queue[MASTER_THREAD_DATA], &msg, MAX_DATA_LEN);
						}
						else
						{
							printf("sim fail throw\n");
							continue;
						}
					}
				}
			}
		}
	}

	return ret;
}