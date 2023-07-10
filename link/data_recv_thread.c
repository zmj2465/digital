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
	for (i = 0; i < info.simulated_link_num && FD[i].fd != 0; i++)
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
			ret = recv(FD[i].fd, FD[i].recvBuffer, sizeof(psy_msg_t), 0);
			if (ret > 0)
			{
				/*状态机判断*/
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

					ret = antenna_match(psy_msg, &msg, info.device_info.node_role);
					if (ret == 1)
					{

						enqueue(&info.thread_queue[MASTER_THREAD_DATA], &msg, MAX_DATA_LEN);
					}
					else
					{
						printf("match fail throw, M send antenna = %d, Z%d receive antenna = %d, current slot = %d.%d\n", psy_msg->msg.head.antenna_id, MY_INDEX, inquire_antenna(info.current_slot), info.current_time_frame, info.current_slot);
					}
					///*信道仿真,匹配自身波束信息对齐*/
					//ret = psy_recv(len, psy_msg, &msg, antenna_recv, info.device_info.node_role);
					//if (ret == 0)
					//{
					//	enqueue(&info.thread_queue[MASTER_THREAD_DATA], &msg, MAX_DATA_LEN);
					//}
					//else
					//{
					//	printf("sim fail throw\n");
					//	continue;
					//}
				}
			}
			else
			{
				printf("node %d link error\n", i);
				FD[i].fd = 0;
				msg_t msg;
				msg.data[0] = i;
				enqueue(&info.thread_queue[LINK_CONTROL_THREAD], &msg, sizeof(msg_t));
			}
		}
	}

	return ret;
}


int antenna_match(char* data, msg_t* msg, int role)
{
	int antenna_recv;
	int index;
	psy_msg_t* ptr = (psy_msg_t*)data;

	if(role == 0)//M
	{
		if (ptr->msg.head.type == SCAN && ptr->msg.data[0] == SCAN_RES)
		{
			index = inquire_address(ptr->msg.head.src);
			info.antenna_M[index] = ptr->msg.head.antenna_id;
		}
		memcpy(msg, (msg_t*)(&ptr->msg), sizeof(msg_t));
		return 1;
	}
	else//Z
	{
		if (ptr->msg.head.type == SCAN && ptr->msg.data[0] == SCAN_REQ)
		{
			antenna_recv = inquire_antenna(info.current_slot);
		}
		else
		{
			antenna_recv = info.antenna_Z;
		}

		if (ptr->msg.head.antenna_id == antenna_recv)
		{
			uint64_t recv;
			int sub;
			memcpy(msg, (msg_t*)(&ptr->msg), sizeof(msg_t));
			info.antenna_Z = antenna_recv;

			recv = my_get_time();
			sub = recv - msg->head.send_t;

			printf("temp = %d ns\n", sub/1000);
			printf("match successfully, M send antenna = %d, Z%d receive antenna = %d, current slot = %d.%d\n", ptr->msg.head.antenna_id, MY_INDEX, info.antenna_Z, info.current_time_frame, info.current_slot);
			return 1;
		}
	}
	
	return 0;
}