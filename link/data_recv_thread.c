#include "data_recv_thread.h"


void* data_recv_thread(void* arg)
{
    pthread_detach(pthread_self());


    char data[MAX_DATA_LEN];
    int len;

    while (1)
    {
        /*接收消息*/
		recv_proc();

        /*信道仿真*/

        /*送往上层*/
        enqueue(&info.thread_queue[MASTER_THREAD], data, len);
    }
}


int recv_proc()
{
	int ret = 0;
	FD_ZERO(&RSET);
	int i = 0;
	for (i = 0; i < info.simulated_link_num; i++)
	{
		FD_SET(FD[i].fd, &RSET);
	}

	ret = select(info.simulated_link_num, &RSET, NULL, NULL, NULL);
	if (ret < 0)
	{
		ret = -1;
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
					printf("get %d info %s\r\n", i, FD[i].recvBuffer);
				}
			}
		}
	}

	return ret;
}