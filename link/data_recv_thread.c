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
int data_recv_proc()
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
					start_boardcast_t* time = FD[i].recvBuffer;
					printf("base time=%lld,%ld start time=%d\n", time->base_time.tv_sec, time->base_time.tv_nsec, time->start_time);
					
					/*����master�̵߳����ݶ���*/
					msg_t* msg = FD[i].recvBuffer;
					enqueue(&info.thread_queue[MASTER_THREAD_DATA], msg, msg->len);
					printf("dst = %d\n", msg->head.dst);
				}
			}
		}
	}

	return ret;
}