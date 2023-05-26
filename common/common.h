#ifndef _COMMON_H_
#define _COMMON_H_

#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <stdint.h>


#include "compatible.h"
#include "queue.h"

#define aaaaaa 20
#define test	20

#define MAX_DEVICE	20
#define HOST_NAME_LEN	20
#define IP_LEN			20
#define MAX_DATA_LEN 1024
#define MAX_TEST 5000

#define MY_INDEX info.link_index
#define FD		info.simulated_link
#define FD_NUM  info.simulated_link_num
#define RSET    info.rset


#define LFD  info.simulated_link[MY_INDEX].fd

enum {
	RS485_RECV_THREAD,
	RS485_SEND_THREAD,
	FDDI_THREAD,
	SCHEDULE_THREAD,
	CONTROL_RECV_THREAD,
	DISPLAY_SEND_THREAD,
	MASTER_THREAD,
	LINK_CONTROL_THREAD,
	DATA_SEND_THREAD,
	DATA_RECV_THREAD,
	END_THREAD
};

typedef struct _link_info_t
{
	int					fd;
	int                 index;
	struct sockaddr_in  addr;
	char				host_name[HOST_NAME_LEN];  //设备名
	char				ip[IP_LEN];				//设备ip
	socklen_t			addr_len;
	char				recvBuffer[MAX_DATA_LEN];
	char				sendBuffer[MAX_DATA_LEN];
}link_info_t;


typedef struct _device_info_t
{
	int  id;						//设备id
	int  role;						//设备角色
	//char host_name[HOST_NAME_LEN];  //设备名
	//char ip[IP_LEN];				//设备ip

}device_info_t;


typedef struct _info_t
{
	/* */
	pthread_t rs_485_recv_thread_id;
	pthread_t rs_485_send_thread_id;

	pthread_t fddi_thread_id;

	pthread_t schedule_thread_id;

	pthread_t control_recv_thread_id;
	pthread_t display_send_thread_id;

	pthread_t master_thread_id;

	pthread_t link_control_thread_id;
	pthread_t data_send_thread_id;
	pthread_t data_recv_thread_id;

	/* */
	sem_t send_semaphore;

	/* */
	msg_queue_t thread_queue[END_THREAD];

	/* */
	link_info_t simulated_link[MAX_DEVICE];
	fd_set rset;
	fd_set wset;
	int simulated_link_num;
	int link_index;
	char ip[IP_LEN];				//设备ip
	
	/* */
	link_info_t control_system;
	link_info_t display_system;
	link_info_t fddi_system;
	char control_ip[IP_LEN];
	char fddi_ip[IP_LEN];
	char display_ip[IP_LEN];
	int communication_port;
	int fddi_port;
	int control_port;
	int display_port;

}info_t;


typedef struct _mmsg_t
{
	int len;
	void* data;
}mmsg_t;

typedef struct _head_t
{
	char dst;
	char src;
	char type;
}head_t;

typedef struct _cp_t
{
	char ad;

}cp_t;


//new
typedef struct _start_boardcast_t
{
	struct timespec base_time;
	int start_time;
}start_boardcast_t;


extern info_t info;


void queue_init();



#endif

