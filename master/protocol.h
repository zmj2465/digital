#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "common.h"
#include "schedule_thread.h"
#include "data_send_thread.h"
#include "physical_simulation.h"

typedef enum _FSM_TYPE
{
	FSM_INIT,	/*simulate network init*/
	FSM_OFF,	/*not build chain*/
	FSM_WSN,	/*wait set network*/
	FSM_WAN,	/*wait access network*/
	FSM_ON		/*set network successfully*/
}FSM_TYPE;

typedef enum _EVENT_TYPE
{
	EVENT_INIT,
	EVENT_WAIT_SIMULATE,
	EVENT_WAIT_ACCESS,
	EVENT_WSN_SUCC,
	EVENT_WAN_SUCC,
	EVENT_LOST_Z,
	EVENT_LOST_M
}EVENT_TYPE;

typedef struct _trans_t
{
	EVENT_TYPE  event;
	int			(*judge)(int);/*check the condition whether it satisfied of status transfer*/
	int			(*oprst)(int);/*start operation before status change*/
	int			(*opred)(int);/*end operation after status change*/
	FSM_TYPE	next;
}trans_t;

typedef struct _fsm_t
{
	char	 name[32];
	int		 num;
	trans_t* test;
}fsm_t;

extern FSM_TYPE fsm_status;
extern void fsm_do(int event);

int fsm_null_cond(int para);
int fsm_init2off_st(int para);
int fsm_init2off_ed(int para);
int fsm_off2wsn_st(int para);
int fsm_off2wsn_ed(int para);
int fsm_off2wan_st(int para);
int fsm_off2wan_ed(int para);
int fsm_wsn2on_st(int para);
int fsm_wsn2on_ed(int para);
int fsm_wan2on_st(int para);
int fsm_wan2on_ed(int para);
int fsm_on2wsn_st(int para);
int fsm_on2wsn_ed(int para);
int fsm_on2wan_st(int para);
int fsm_on2wan_ed(int para);

#endif