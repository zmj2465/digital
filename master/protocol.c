#include "protocol.h"
#include "fddi_thread.h"

FSM_TYPE fsm_status = FSM_INIT;

static trans_t fsm_init[] =
{
	{EVENT_INIT, &fsm_null_cond, &fsm_init2off_st, &fsm_init2off_ed, FSM_OFF}
};

static trans_t fsm_off[] =
{
	{EVENT_WAIT_SIMULATE, &fsm_null_cond, &fsm_off2wsn_st, &fsm_off2wsn_ed, FSM_WSN},
	{EVENT_WAIT_ACCESS, &fsm_null_cond, &fsm_off2wan_st, &fsm_off2wan_ed, FSM_WAN}
};

static trans_t fsm_wsn[] =
{
	{EVENT_WSN_SUCC, &fsm_null_cond, &fsm_wsn2on_st, &fsm_wsn2on_ed, FSM_ON}
};

static trans_t fsm_wan[] =
{
	{EVENT_WAN_SUCC, &fsm_null_cond, &fsm_wan2on_st, &fsm_wan2on_ed, FSM_ON}
};

static trans_t fsm_on[] =
{
	{EVENT_LOST_Z, &fsm_null_cond, &fsm_on2wsn_st, &fsm_on2wsn_ed, FSM_WSN},
	{EVENT_LOST_M, &fsm_null_cond, &fsm_on2wan_st, &fsm_on2wan_ed, FSM_WAN}
};

static fsm_t fsm[] =
{
	{"fsm_init", sizeof(fsm_init) / sizeof(fsm_init[0]), fsm_init},
	{"fsm_off",  sizeof(fsm_off) / sizeof(fsm_off[0]),   fsm_off},
	{"fsm_wsn",  sizeof(fsm_wsn) / sizeof(fsm_wsn[0]),	 fsm_wsn},
	{"fsm_wan",  sizeof(fsm_wan) / sizeof(fsm_wan[0]),	 fsm_wan},
	{"fsm_on",	 sizeof(fsm_on) / sizeof(fsm_on[0]),	 fsm_on}
};

/*
功能：状态机状态转移变化
参数：触发状态转移的事件
返回值：无
*/
void fsm_do(int event)
{
	int i, num;
	trans_t* ptr;
	int con, oprst, opred;
	ptr = fsm[fsm_status].test;
	num = fsm[fsm_status].num;
	for (i = 0; i < num; i++)
	{
		if (ptr[i].event == event)
		{
			con = (*ptr[i].judge)(event);//目前给的是空判断条件，condition恒等于0
			oprst = (*ptr[i].oprst)(event);
			if (con == 0 && oprst == 0)//0表示满足转移条件
			{
				fsm_status = ptr[i].next;//设置新状态	
				printf("fsm new status : %s, event : %d\r\n", fsm[fsm_status].name, ptr[i].event);
				opred = (*ptr[i].opred)(event);
				break;
			}
			else
			{
				printf("oprst error,status transition failed\r\n");
			}
		}
	}
}

/*
功能：判断状态转移的条件
参数：无
返回值：0表示成功
*/
int fsm_null_cond(int para)
{
	return 0;
}

/*
功能：初始状态->未在网状态（之前）
参数：无
返回值：0表示成功
*/
int fsm_init2off_st(int para)
{
	int i;
	for (i = 0; i < FD_NUM; i++)
	{
		info.device_info.node_id[i] = (i | 16);
	}
	if (MY_INDEX == 0)
	{
		info.device_info.node_role = 0;
	}
	else
	{
		info.device_info.node_role = 1;
	}
	info.device_info.node_num = 1;
	info.device_info.node_list = 0x21;
	schedule_slot_init();
	info.seq_data = 0;
	info.seq_beacon = 0;
	info.seq_distance = 0;
	info.time_schedule_flag = 0;
	info.chain_flag_m = 0;
	info.chain_flag_z = 0;
	memset(&display_data, 0, sizeof(data_t));
	//sql_init();
	return 0;
}

/*
功能：初始状态->未在网状态（之后）
参数：无
返回值：0表示成功
*/
int fsm_init2off_ed(int para)
{
	printf("等待建链命令\n");
	//info.chain_flag_m = 1;
	while (1)
	{
		if (info.chain_flag_m == 1)//收到建链命令
		{
			/*主机发送信令枪帧*/
			if (MY_INDEX == 0)
			{
#ifdef _WIN32
				Sleep(1000);
#endif
				msg_t msg;
				int i;
				msg.data[0] = START_GUN_REQ;
				msg.data[1] = START_GUN_TIME;
				msg.len = 2;
				//clock_gettime(CLOCK_REALTIME, &info.str.base_time);
				info.str.base_t = my_get_time();
				info.str.start_time = START_GUN_TIME;
				for (i = 1; i < FD_NUM; i++)
				{
					generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], START_GUN, &msg);
					send(FD[i].fd, &msg, msg.len, 0);
				}
				//plog("M base time=%lld, %ld, start_time = %d\n", info.str.base_time.tv_sec, info.str.base_time.tv_nsec, info.str.start_time);
				printf("M base time=%lld ns, start_time = %d s\n", info.str.base_t, info.str.start_time);
#ifdef _WIN32
				info.timerId = timeSetEvent(TIMER_DELAY, 0, TimerCallback, START_GUN_TIMER, TIME_ONESHOT);
#endif
				generate_key_event(0, 0, 0);
				Sleep(1000);
				generate_key_event(1, 0, 0);
				Sleep(1000);
				generate_key_event(2, 1, 1);
				Sleep(1000);
				send_start();
				generate_key_event(3, 0, 0);
				Sleep(1000);
			}
			break;
		}
	}
	
	return 0;
}

/*
功能：未在网状态->等待网络建立状态（只针对终端M）（之前）
参数：无
返回值：0表示成功
*/
int fsm_off2wsn_st(int para)
{
	/*同步仿真建链时间，将自身状态设置为fsm_wsn*/
	//struct timespec str_m;
	//clock_gettime(CLOCK_REALTIME, &str_m);
	//while ((str_m.tv_sec * 1000000000 + str_m.tv_nsec) < (info.str.base_time.tv_sec * 1000000000 + info.str.base_time.tv_nsec + info.str.start_time * 1000000000))
	//{
	//	clock_gettime(CLOCK_REALTIME, &str_m);
	//}
	uint64_t str;
	str = my_get_time();
	while (str < (info.str.base_t + info.str.start_time * 1000000000))
	{
		str = my_get_time();
	}
	printf("M start,send scan ask\n");
	return 0;
}

/*
功能：未在网状态->等待网络建立状态（只针对终端M）（之后）
参数：无
返回值：0表示成功
*/
int fsm_off2wsn_ed(int para)
{
	/*计算建网时间，开始时刻*/
	//clock_gettime(CLOCK_REALTIME, &info.set_network_st);
	info.network_st = my_get_time();
	generate_key_event(5,0,0);
	return 0;
}

/*
功能：未在网状态->等待入网状态（只针对终端Z）（之前）
参数：无
返回值：0表示成功
*/
int fsm_off2wan_st(int para)
{
	/*同步仿真建链时间，将自身状态设置为fsm_wan*/
	//struct timespec str_m;
	//clock_gettime(CLOCK_REALTIME, &str_m);
	//while ((str_m.tv_sec * 1000000000 + str_m.tv_nsec) < (info.str.base_time.tv_sec * 1000000000 + info.str.base_time.tv_nsec + info.str.start_time * 1000000000))
	//{
	//	clock_gettime(CLOCK_REALTIME, &str_m);
	//}
	generate_key_event(0,0,0);
	Sleep(1000);
	generate_key_event(1,0,0);
	Sleep(1000);
	generate_key_event(2,1,1);
	Sleep(1000);
	generate_key_event(3,0,0);
	Sleep(1000);
	uint64_t str;
	str = my_get_time();
	while (str < (info.str.base_t + info.str.start_time * 1000000000))
	{
		str = my_get_time();
	}
	return 0;
}

/*
功能：未在网状态->等待入网状态（只针对终端Z）（之后）
参数：无
返回值：0表示成功
*/
int fsm_off2wan_ed(int para)
{
	/*打开扫描请求定时器*/
#ifdef _WIN32
	info.timerId = timeSetEvent(TIMER_DELAY, 0, TimerCallback, SCAN_REQ_TIMER, TIME_ONESHOT);
#endif
	return 0;
}

/*
功能：等待网络建立状态->网络建立完成状态（只针对终端M）（之前）
参数：无
返回值：0表示成功
*/
int fsm_wsn2on_st(int para)
{
	/*计算建网时间，结束时刻*/
	uint64_t sub;
	//clock_gettime(CLOCK_REALTIME, &info.set_network_ed);
	//sub = (info.set_network_ed.tv_sec - info.set_network_st.tv_sec) * 1000000000 + (info.set_network_ed.tv_nsec - info.set_network_st.tv_nsec);
	info.network_ed = my_get_time();
	sub = info.network_ed - info.network_st;
	info.set_network_time = sub / 1000000;
	plog("set network time = %d ms\n", info.set_network_time);
	return 0;
}

/*
功能：等待网络建立状态->网络建立完成状态（只针对终端M）（之后）
参数：无
返回值：0表示成功
*/
int fsm_wsn2on_ed(int para)
{
	return 0;
}

/*
功能：等待入网状态->入网成功状态（只针对终端Z）（之前）
参数：无
返回值：0表示成功
*/
int fsm_wan2on_st(int para)
{
	return 0;
}

/*
功能：等待入网状态->入网成功状态（只针对终端Z）（之后）
参数：无
返回值：0表示成功
*/
int fsm_wan2on_ed(int para)
{
	return 0;
}

/*
功能：在网状态->等待建网状态（只针对终端M）（之前）
参数：无
返回值：0表示成功
*/
int fsm_on2wsn_st(int para)
{
	return 0;
}

/*
功能：在网状态->等待建网状态（只针对终端M）（之后）
参数：无
返回值：0表示成功
*/
int fsm_on2wsn_ed(int para)
{
	/*丢失再建链，计算建网时间，开始时刻*/
	//clock_gettime(CLOCK_REALTIME, &info.set_network_st);
	info.network_st = my_get_time();
	return 0;
}


/*
功能：在网状态->等待入网状态（只针对终端Z）（之前）
参数：无
返回值：0表示成功
*/
int fsm_on2wan_st(int para)
{
	return 0;
}

/*
功能：在网状态->等待入网状态（只针对终端Z）（之后）
参数：无
返回值：0表示成功
*/
int fsm_on2wan_ed(int para)
{
	return 0;
}