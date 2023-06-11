#ifndef _SCHEDULE_THREAD_H_
#define _SCHEDULE_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "compatible.h"
#include "common.h"
#include "protocol.h"

#define ANTENNA_NUM   6   /*天线数*/
#define SLOT_NUM	  63  /*时隙数*/
	
typedef enum _SLOT_NAME
{
	M1_SEND_Z1,
	M1_SEND_Z2,
	M1_SEND_Z3,
	M1_SEND_Z4,
	M1_SEND_M2,
	M1_SNED_Z5,
	PROTECT1,
	Z1_SEND_M1,
	Z2_SEND_M1,
	Z3_SEND_M1,
	Z4_SEND_M1,
	Z5_SEND_M1,
	M2_SEND_M1,
	Z_DISTANCE_M1,
	PROTECT2,
	M1_DISTANCE_Z,
	RESERVE,
}SLOT_NAME;

void* schedule_thread(void* arg);
void schedule_slot_init(void);
int schedule_slot(void);
int schedule_inquire_index(int index, int current_slot);
int inquire_index(int node_index);
int inquire_slot(int current_slot);
void CALLBACK TimerCallback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2);

#endif



//typedef struct _slot_event_t
//{
//	int slot;
//}slot_event_t;
//
//typedef struct _slot_table_t
//{
//	slot_event_t* ptr;
//	int num;			//指示SLOT_NUM_J的长度
//	int antenna_id;
//	int antenna_status; //0接收1发送
//	int current_slot_i;
//	int slot[SLOT_NUM_J];
//	int current_slot;
//}slot_table_t;