#include "master_thread.h"
#include "common.h"

/*
功能：主控线程
参数：无
返回值：无
*/
void* master_thread(void* arg)
{
	pthread_detach(pthread_self());

	while (1)
	{
		/*控制队列处理*/

		/*数据队列处理*/
		master_data_proc();

	}
}

/*
功能：主控线程数据队列的处理
参数：无
返回值：无
*/
int master_data_proc(void)
{
	int index;
	msg_t msg;
	uint8_t node_id;
	dequeue(&info.thread_queue[MASTER_THREAD_DATA], &msg, &msg.len);
	/*解包*/
	msg.len = msg.len - sizeof(head_t) - sizeof(int);

	if (msg.head.dst == MY_ID)
	{
		/*主机*/
		if (MY_INDEX == 0)
		{
			switch (msg.head.type)
			{
			case SHORT_FRAME:
				break;
			case BEACON:
				index = inquire_address(msg.head.src);
#ifdef _WIN32
				/*信令帧M定时器*/
				timeKillEvent(info.timerId_M[index]);
				info.timerId_M[index] = timeSetEvent(TIMER_DELAY, 0, TimerCallback, index, TIME_ONESHOT);
#endif
				plog("M recv Z%d beacon, current slot = %d.%d, seq = %d\n", index, info.current_time_frame, info.current_slot, msg.head.seq);
				break;
			case LONG_FRAME:
				index = inquire_address(msg.head.src);

				if (msg.data[0] == GUI_FRAME)//制导
				{
					printf("m get gui\n");
					put(&common_data[M_GUI_RECV], msg.data, M_GUI_RECV_LEN, index);
					return_flag = M_GUI_RECV_LEN;
				}
				if (msg.data[0] == TOM_FRAME)//TOM
				{
					printf("m get tom\n");
					put(&common_data[M_TOM_RECV], msg.data, M_TOM_RECV_LEN, index);
					return_flag = M_TOM_RECV_LEN;
				}
				g_node_progrm[index].air_interface_data_rx_count++;
				plog("M recv Z%d data, current slot = %d.%d, seq = %d\n", index, info.current_time_frame, info.current_slot, msg.head.seq);
				break;
			case START_GUN:
				
				if (msg.data[0] == START_GUN_RES)
				{
#ifdef _WIN32
					timeKillEvent(info.timerId);//关闭发令枪定时器
#endif
					fsm_do(EVENT_WAIT_SIMULATE);
				}
				break;
			case SCAN:
				if (msg.data[0] == SCAN_RES)
				{
					index = inquire_address(msg.head.src);
					info.device_info.node_num++;
					info.device_info.node_list = info.device_info.node_list | (1 << index);
					info.scan_flag_M[index] = 1;
					generate_key_event(KEY_LINK_ON, index, 1);
					plog("M recv Z%d scan response, list = %d, current slot = %d.%d, seq = %d\n", index, info.device_info.node_list, info.current_time_frame, info.current_slot, msg.head.seq);
				}
				break;
			case DISTANCE:
				if (msg.data[0] == DISTANCE_Z)
				{
					index = inquire_address(msg.head.src);
					info.distance_flag_M[index] = 1;
					plog("M recv Z%d distance frame, current slot = %d.%d, seq = %d\n", index, info.current_time_frame, info.current_slot, msg.head.seq);
				}
				break;
			case PARAMETER_LOAD:
				index = msg.data[0];
				node_id = msg.data[1];
				info.device_info.node_id[index] = node_id;
				printf("ok %d\n",node_id);
				break;
			default:
				break;
			}
		}
		/*从机*/
		else
		{
			//printf("seq=%d\n", msg.head.seq);
			switch (msg.head.type)
			{
			case START_GUN:

				if ((msg.data[0] == START_GUN_REQ) && (info.chain_flag_z == 1))//z收到建链标志和发令枪请求后，才开始回发令枪响应
				{
					info.str.start_time = msg.data[1];
					info.str.base_t = msg.head.send_t;
					printf("Z%d base time=%lld ns, start time = %d s\n", MY_ID_INDEX, info.str.base_t, info.str.start_time);
					//info.str.base_time = msg.head.send_time;
					//plog("Z%d base time=%lld, %ld, start_time = %d\n", MY_INDEX, info.str.base_time.tv_sec, info.str.base_time.tv_nsec, info.str.start_time);
					/*响应主机的发令枪帧*/
					msg.data[0] = START_GUN_RES;
					msg.len = 1;
					generate_packet(info.device_info.node_id[0], MY_ID, START_GUN, &msg);
					send(FD[0].fd, &msg, sizeof(msg_t), 0);
					fsm_do(EVENT_WAIT_ACCESS);
				}
				break;
			case SCAN:
				if (msg.data[0] == SCAN_REQ)
				{
#ifdef _WIN32
					timeKillEvent(info.timerId);//关闭扫描请求定时器
#endif
					info.scan_flag_Z = 1;//扫描响应标志位置1
					plog("Z%d recv M scan request, current slot = %d.%d, seq = %d\n", MY_ID_INDEX, info.current_time_frame, info.current_slot, msg.head.seq);
				}
				else if (msg.data[0] == SCAN_CON)
				{
#ifdef _WIN32
					timeKillEvent(info.timerId);//关闭扫描确认定时器
#endif
					info.time_frame_flag_z = msg.data[1];
					plog("Z%d recv M scan confirm, current slot = %d.%d, seq = %d\n", MY_ID_INDEX, info.current_time_frame, info.current_slot, msg.head.seq);
					generate_key_event(KEY_LINK_ON, 0, 0);
					fsm_do(EVENT_WAN_SUCC);
				}
				break;
			case BEACON:
#ifdef _WIN32
				/*信令帧Z定时器*/
				timeKillEvent(info.timerId_Z);
				info.timerId_Z = timeSetEvent(TIMER_DELAY, 0, TimerCallback, Z_DATA_TIMER, TIME_ONESHOT);
#endif
				plog("Z%d recv M beacon, current slot = %d.%d, seq = %d\n", MY_ID_INDEX, info.current_time_frame, info.current_slot, msg.head.seq);
				break;
			case LONG_FRAME:
				if (msg.data[0] == GUI_FRAME)//制导
				{
					printf("z get gui\n");
					return_flag = Z_GUI_RECV_LEN;
					put(&common_data[Z_GUI_RECV], msg.data, Z_GUI_RECV_LEN, 0);
				}
				if (msg.data[0] == TOM_FRAME)//TOM
				{
					printf("z get tom\n");
					return_flag = Z_TOM_RECV_LEN;
					put(&common_data[Z_TOM_RECV], msg.data, Z_TOM_RECV_LEN, 0);
				}
				g_node_progrm[0].air_interface_data_rx_count++;
				plog("Z%d recv M data, current slot = %d.%d, seq = %d\n", MY_ID_INDEX, info.current_time_frame, info.current_slot, msg.head.seq);
				break;
			case DISTANCE:
				plog("Z%d recv M distance frame, current slot = %d.%d, seq = %d\n", MY_ID_INDEX, info.current_time_frame, info.current_slot, msg.head.seq);
				break;
			case PARAMETER_LOAD:

				break;
			default:
				break;
			}
		}
	}
	else
	{
		//plog("dst uncorrectly, master thread receive failed\n");
	}

	return 0;
}

/*
功能：主控线程控制队列的处理
参数：无
返回值：无
*/
int master_control_proc(void)
{

	return 0;
}