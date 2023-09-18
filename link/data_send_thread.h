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
	M_GUI_SEND,
	M_GUI_RECV,
	M_TOM_SEND,
	M_TOM_RECV,
	Z_GUI_SEND,
	Z_GUI_RECV,
	Z_TOM_SEND,
	Z_TOM_RECV,
	MZ_DATA,
};

#define M_GUI_SEND_LEN 33
#define M_GUI_RECV_LEN 46
#define M_TOM_SEND_LEN 456
#define M_TOM_RECV_LEN 468

#define Z_GUI_SEND_LEN 46
#define Z_GUI_RECV_LEN 33
#define Z_TOM_SEND_LEN 468
#define Z_TOM_RECV_LEN 456

#pragma pack(1)

typedef struct _m_gui_send_t
{
	uint8_t data[33 * 4];
	pthread_mutex_t lock;
}m_gui_send_t;

typedef struct _m_gui_recv_t
{
	uint8_t data[46 * 4];
	pthread_mutex_t lock;
}m_gui_recv_t;

typedef struct _m_tom_send_t
{
	uint8_t data[456 * 4];
	pthread_mutex_t lock;
}m_tom_send_t;

typedef struct _m_tom_recv_t
{
	uint8_t data[468 * 4];
	pthread_mutex_t lock;
}m_tom_recv_t;


typedef struct _z_gui_send_t
{
	uint8_t data[46];
	pthread_mutex_t lock;
}z_gui_send_t;

typedef struct _z_gui_recv_t
{
	uint8_t data[33];
	pthread_mutex_t lock;
}z_gui_recv_t;

typedef struct _z_tom_send_t
{
	uint8_t data[468];
	pthread_mutex_t lock;
}z_tom_send_t;

typedef struct _z_tom_recv_t
{
	uint8_t data[456];
	pthread_mutex_t lock;
}z_tom_recv_t;


typedef struct _common_data_t {
	uint8_t data[2048];
	pthread_mutex_t lock;
}common_data_t;


#pragma pack()


extern m_gui_send_t m_gui_send;
extern m_gui_recv_t m_gui_recv;
extern m_tom_send_t m_tom_send;
extern m_tom_recv_t m_tom_recv;

extern z_gui_send_t z_gui_send;
extern z_gui_recv_t z_gui_recv;
extern z_tom_send_t z_tom_send;
extern z_tom_recv_t z_tom_recv;



extern common_data_t common_data[MZ_DATA];


void get(common_data_t* src, char* dst, int len, int index);
void put(common_data_t* dst, char* src, int len, int index);


void* data_send_thread(void* arg);
int   data_send_proc(void);
void  generate_packet(uint8_t dst, uint8_t src, uint8_t type, msg_t* msg);




#endif