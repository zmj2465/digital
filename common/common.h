#ifndef _COMMON_H_
#define _COMMON_H_

#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <stdint.h>
#include "compatible.h"
#include "queue.h"

#define STORE_SIZE		1024
#define MAX_DEVICE		12
#define HOST_NAME_LEN	20
#define IP_LEN			20
#define MAX_DATA_LEN	1024
#define START_GUN_TIME	10		/*仿真开始时间,暂定10s后*/
#define TIMER_DELAY		300		/*定时器延时：300ms*/
#define MY_INDEX		info.link_index
#define FD				info.simulated_link
#define FD_NUM			info.simulated_link_num
#define RSET			info.rset
#define LFD				info.simulated_link[MY_INDEX].fd

enum 
{
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
	DATA_SEND_THREAD_SCAN,
	END_THREAD
};

enum FRAME_TYPE
{
	SLEF_TEST		 = 0x00, //自检
	SLEF_TEST_RESULT = 0x10, //自检结果
	PARAMETER_LOAD	 = 0xcc, //参数装订
	SHORT_FRAME		 = 0x30, //短帧
	LONG_FRAME		 = 0x60, //长帧
	START_GUN		 = 0x70, //发令枪
	SCAN			 = 0x80, //扫描
};

enum FRAME_SUBTYPE
{
	START_GUN_REQ = 1,
	START_GUN_RES = 2,
	SCAN_REQ = 1,
	SCAN_RES = 2,
	SCAN_CON = 3,
};

enum TIMER_ID
{
	SCAN_REQ_TIMER,
	SCAN_RES_TIMER,
	SCAN_CON_TIMER,
	DATA_TIMER
};

typedef struct _link_info_t
{
	int					fd;
	int                 index;
	struct sockaddr_in  addr;
	char				host_name[HOST_NAME_LEN];  //设备名
	char				ip[IP_LEN];				//设备ip
	int                 port;
	socklen_t			addr_len;
	uint8_t				recvBuffer[MAX_DATA_LEN*2];
	char				sendBuffer[MAX_DATA_LEN*2];
}link_info_t;


typedef struct _device_info_t
{
	uint8_t node_role;				//节点角色（00H：M，01H：Z）
	uint8_t node_id[MAX_DEVICE];	//节点ID（10H：M，11H：Z1，12H：Z2，13H：Z3，14H：Z4）
	uint8_t node_num;				//网络节点个数（01-05H）
	uint8_t node_list;				//网络节点ID列表（前3位表示个数，后5位表示网络在网节点，0为不在网，1为在网）
	uint8_t freqC;					//扩频码组（1-8）
	uint8_t freqP;					//跳频序列（1-8）
}device_info_t;

typedef struct _start_boardcast_t
{
	struct timespec base_time;		/*同步系统时间*/
	uint8_t start_time;				/*仿真开始时间*/
}start_boardcast_t;

typedef struct _info_t
{
	/*all_thread_id*/
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

	/*all_semaphore*/
	sem_t send_semaphore;
	sem_t thread_create_semaphore;

	/*all_msg_queue*/
	msg_queue_t thread_queue[END_THREAD];

	/*communication_interface*/
	link_info_t simulated_link[MAX_DEVICE];
	fd_set rset;
	fd_set wset;
	int simulated_link_num;
	int link_index;
	char ip[IP_LEN];				//样机设备ip
	int port;
	
	/*external_interface*/
	link_info_t control_system;
	link_info_t display_system;
	link_info_t fddi_system;
	char control_ip[IP_LEN];
	char fddi_ip[IP_LEN];
	char display_ip[IP_LEN];
	//int communication_port;
	int fddi_port;
	int control_port;
	int display_port;

	/*simulation_replay*/
	HANDLE hSharedMem;
	LPVOID lpSharedMem;
	int mem_ptr;
	int act_prt;

	/*device_info*/
	device_info_t device_info;
	start_boardcast_t str;
	int current_slot;
	int current_antenna;
	uint8_t scan_flag[MAX_DEVICE];
	UINT timerId;
	UINT timerId_M[MAX_DEVICE];
	int antenna_M[MAX_DEVICE];
	int antenna_Z;
}info_t;

typedef struct _head_t
{
	uint8_t dst;
	uint8_t src;
	uint8_t type;
	struct timespec send_time;
	uint8_t antenna_id;
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
	uint8_t data[MAX_DATA_LEN]; //样机内部数据只传输data
	int		len;				//样机内部传输时不含head长度，样机间传输时包含head长度
}msg_t;

extern info_t info;

typedef struct {
	float x;
	float y;
	float z;
} Point3D;

typedef struct {
	float q0;
	float q1;
	float q2;
	float q3;
} Quaternion;

typedef struct {
	float dx;
	float dy;
	float dz;
	float h[3][3];
} AntennaTransform;


extern AntennaTransform transform[12];

typedef struct _fddi_info_t
{
	char a;
	char b;
	char c;
	char ad;  //1

	int rti;  //4
	int tf;   //4
	int rtig; //4
	Point3D pos;
	Point3D v;
	Point3D rv;
	Quaternion q;
}fddi_info_t;


typedef struct _psy_msg_t
{
	Point3D pos;
	Point3D p_to;
	Quaternion q;
	int role;
	int index;
	int flag;
	msg_t msg;
}psy_msg_t;



extern fddi_info_t fddi_info;


void queue_init();


#endif

