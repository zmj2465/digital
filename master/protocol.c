#include "protocol.h"

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
	{"fsm_off",  sizeof(fsm_off) / sizeof(fsm_off[0]),  fsm_off},
	{"fsm_wsn",  sizeof(fsm_wsn) / sizeof(fsm_wsn[0],	 fsm_wsn)},
	{"fsm_wan",  sizeof(fsm_wan) / sizeof(fsm_wan[0],	 fsm_wan)},
	{"fsm_on",	 sizeof(fsm_on) / sizeof(fsm_on[0]),	 fsm_on}
};

/*
���ܣ�״̬��״̬ת�Ʊ仯
����������״̬ת�Ƶ��¼�
����ֵ����
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
			con = (*ptr[i].judge)(event);//Ŀǰ�����ǿ��ж�������condition�����0
			oprst = (*ptr[i].oprst)(event);
			if (con == 0 && oprst == 0)//0��ʾ����ת������
			{
				fsm_status = ptr[i].next;//������״̬
				opred = (*ptr[i].opred)(event);
				printf("fsm new status : %s, event : %d, end opreation ret : %d\r\n", fsm[fsm_status].name, ptr[i].event, opred);
				break;
			}
			else
			{
				printf("oprst error,status transition failed\r\n");
			}
		}
		else
		{
			continue;
		}
	}
}

/*
���ܣ��ж�״̬ת�Ƶ�����
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_null_cond(int para)
{

	return 0;
}

/*
���ܣ�״̬����ʼ״̬�ĳ�ʼ��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_init_do(int para)
{
	return 0;
}

/*
���ܣ���ʼ״̬->δ����״̬��֮ǰ��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_init2off_st(int para)
{
	int i;
	for (i = 0; i < FD_NUM; i++)
	{
		info.device_info[i].node_id = (i | 16);
	}
	return 0;
}

/*
���ܣ���ʼ״̬->δ����״̬��֮��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_init2off_ed(int para)
{
	/*������������ǹ*/
	if (MY_INDEX == 0)
	{
		uint8_t data[MAX_DATA_LEN];
		int len;
		memset(data, 0, MAX_DATA_LEN);
		data[0] = 1;
		len = 1;
		enqueue(&info.thread_queue[DATA_SEND_THREAD], data, len);
	}
	return 0;
}

/*
���ܣ�δ����״̬->�ȴ����罨��״̬��ֻ����ն�M����֮ǰ��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_off2wsn_st(int para)
{
	return 0;
}

/*
���ܣ�δ����״̬->�ȴ����罨��״̬��ֻ����ն�M����֮��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_off2wsn_ed(int para)
{
	return 0;
}

/*
���ܣ�δ����״̬->�ȴ�����״̬��ֻ����ն�Z����֮ǰ��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_off2wan_st(int para)
{
	return 0;
}

/*
���ܣ�δ����״̬->�ȴ�����״̬��ֻ����ն�Z����֮��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_off2wan_ed(int para)
{
	return 0;
}

/*
���ܣ��ȴ����罨��״̬->���罨�����״̬��ֻ����ն�M����֮ǰ��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_wsn2on_st(int para)
{
	return 0;
}

/*
���ܣ��ȴ����罨��״̬->���罨�����״̬��ֻ����ն�M����֮��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_wsn2on_ed(int para)
{
	return 0;
}

/*
���ܣ��ȴ�����״̬->�����ɹ�״̬��ֻ����ն�Z����֮ǰ��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_wan2on_st(int para)
{
	return 0;
}

/*
���ܣ��ȴ�����״̬->�����ɹ�״̬��ֻ����ն�Z����֮��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_wan2on_ed(int para)
{
	return 0;
}

/*
���ܣ�����״̬->�ȴ�����״̬��ֻ����ն�M����֮ǰ��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_on2wsn_st(int para)
{
	return 0;
}

/*
���ܣ�����״̬->�ȴ�����״̬��ֻ����ն�M����֮��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_on2wsn_ed(int para)
{
	return 0;
}

/*
���ܣ�����״̬->�ȴ�����״̬��ֻ����ն�Z����֮ǰ��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_on2wan_st(int para)
{
	return 0;
}

/*
���ܣ�����״̬->�ȴ�����״̬��ֻ����ն�Z����֮��
��������
����ֵ��0��ʾ�ɹ�
*/
int fsm_on2wan_ed(int para)
{
	return 0;
}