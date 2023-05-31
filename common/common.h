#ifndef _COMMON_H_
#define _COMMON_H_

#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <stdint.h>



#include "compatible.h"
#include "queue.h"


#define STORE_SIZE  1024
#define MAX_DEVICE	12
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
	MASTER_THREAD_DATA,
	END_THREAD
};

enum frame_type
{
	SLEF_TEST = 0x00,
	SLEF_TEST_RESULT = 0x10,
	PARAMETER_LOAD = 0xcc,
	SHORT_FRAME = 0x30,
	LONG_FRAME = 0x60,
	START_GUN = 0x70,
	SCAN_REQ = 0x80,
	SCAN_RES = 0x90,
	SCAN_CON = 0xa0
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
	uint8_t node_role;	//节点角色（00H：M，01H：Z）
	uint8_t node_id;	//节点ID（10H：M，11H：Z1，12H：Z2，13H：Z3，14H：Z4）
	uint8_t node_num;	//网络节点个数（01-05H）
	uint8_t node_list;	//网络节点ID列表（前3位表示个数，后5位表示网络在网节点，0为不在网，1为在网）
	uint8_t freqC;		//扩频码组（1-8）
	uint8_t freqP;		//跳频序列（1-8）
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
	sem_t thread_create_semaphore;

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

	/* */
	HANDLE hSharedMem;
	LPVOID lpSharedMem;
	int mem_ptr;
	int act_prt;

	device_info_t device_info[MAX_DEVICE];
}info_t;

typedef struct _start_boardcast_t
{
	struct timespec base_time;
	int start_time;
}start_boardcast_t;

typedef struct _head_t
{
	uint8_t dst;
	uint8_t src;
	uint8_t type;
	start_boardcast_t sbt;
	//位置信息
}head_t;

typedef struct _cp_t
{
	uint8_t para;
	/*待补充*/
}cp_t;

typedef struct _sp_t
{
	uint16_t st;	/*self test result*/
	/*待补充*/
}sp_t;

typedef struct _frame_t
{
	uint32_t flag_start;
	head_t head;
	cp_t cp;
	uint16_t crc;
	uint32_t flag_end;
}frame_t;

typedef struct _msg_t
{
	head_t	head;
	uint8_t data[MAX_DATA_LEN];//样机内部数据只传输data
	int		len;			//指示data的真实长度，不含head的长度
}msg_t;

extern info_t info;

void queue_init();



#endif

