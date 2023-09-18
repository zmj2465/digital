#ifndef _DATA_SEND_THREAD_H_
#define _DATA_SEND_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"
#include "protocol.h"
#include "schedule_thread.h"
#include "physical_simulation.h"

enum {
	M2Z_GUI_SEND,
	M2Z_GUI_RECV,
	M2Z_TOM_SEND,
	M2Z_TOM_RECV,
	Z2M_GUI_SEND,
	Z2M_GUI_RECV,
	Z2M_TOM_SEND,
	Z2M_TOM_RECV,
};

#pragma pack(1)

typedef struct _m2z_gui_send_t
{
	uint8_t data[33 * 4];
	pthread_mutex_t lock;
}m2z_gui_send_t;

typedef struct _m2z_gui_recv_t
{
	uint8_t data[46 * 4];
	pthread_mutex_t lock;
}m2z_gui_recv_t;

typedef struct _m2z_tom_send_t
{
	uint8_t data[456 * 4];
	pthread_mutex_t lock;
}m2z_tom_send_t;

typedef struct _m2z_tom_recv_t
{
	uint8_t data[468 * 4];
	pthread_mutex_t lock;
}m2z_tom_recv_t;


typedef struct _z2m_gui_send_t
{
	uint8_t data[46];
	pthread_mutex_t lock;
}z2m_gui_send_t;

typedef struct _z2m_gui_recv_t
{
	uint8_t data[33];
	pthread_mutex_t lock;
}z2m_gui_recv_t;

typedef struct _z2m_tom_send_t
{
	uint8_t data[468];
	pthread_mutex_t lock;
}z2m_tom_send_t;

typedef struct _z2m_tom_recv_t
{
	uint8_t data[456];
	pthread_mutex_t lock;
}z2m_tom_recv_t;


#pragma pack()


extern m2z_gui_send_t m2z_gui_send;
extern m2z_gui_recv_t m2z_gui_recv;
extern m2z_tom_send_t m2z_tom_send;
extern m2z_tom_recv_t m2z_tom_recv;

extern z2m_gui_send_t z2m_gui_send;
extern z2m_gui_recv_t z2m_gui_recv;
extern z2m_tom_send_t z2m_tom_send;
extern z2m_tom_recv_t z2m_tom_recv;









void* data_send_thread(void* arg);
int   data_send_proc(void);
void  generate_packet(uint8_t dst, uint8_t src, uint8_t type, msg_t* msg);




#endif