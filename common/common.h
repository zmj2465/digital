#ifndef _COMMON_H_
#define _COMMON_H_

#include <pthread.h>
#include <sched.h>
#include <semaphore.h>
#include <stdint.h>
#include <stdio.h>
#include "mytime.h"
#include "queue.h"
#include "compatible.h"
#include "file_manage.h"
//#include "windows.h"
//#include <dirent.h>


#if(0)
#define plog(format, ...) tofile(LOG,format,##__VA_ARGS__)
#else
#define plog(format, ...) printf(format, ##__VA_ARGS__)
#endif

#define tosche(format, ...) tofile(SCHE,format,##__VA_ARGS__)

extern uint64_t start_time;
extern uint64_t end_time;
//extern pthread_spinlock_t start_spin;

#define STORE_SIZE		1024	//存储文件的大小
#define MAX_DEVICE		12		//最大终端数量
#define HOST_NAME_LEN	20		//设备名长度
#define IP_LEN			20		//ip地址长度
#define MAX_DATA_LEN	2048	//最大数据长度
#define START_GUN_TIME	10		//仿真开始时间：10s
#define TIMER_DELAY		300  	//定时器时间：300ms
#define MY_INDEX		info.link_index
#define MY_ROLE         info.my_role
#define FD				info.simulated_link					
#define FD_NUM			info.simulated_link_num				//连接仿真系统的装置数目
#define RSET			info.rset							
#define LFD				info.simulated_link[MY_INDEX].fd
#define DISPLAY_FD      info.display_system.fd

enum 
{
	DATA,
	LOG,
	SCHE,
};

enum 
{
	RS485_RECV_THREAD,
	RS485_SEND_THREAD,
	FDDI_THREAD,
	SCHEDULE_THREAD,
	CONTROL_RECV_THREAD,
	DISPLAY_SEND_THREAD,
	DISPLAY_RECV_THREAD,
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
	SLEF_TEST		   = 0x00, //自检
	SLEF_TEST_RESULT   = 0x10, //自检结果
	PARAMETER_LOAD	   = 0xcc, //参数装订
	BUILD_CHAIN		   = 0x20, //建链
	BUILD_CHAIN_RESULT = 0x40, //建链结果
	WORK_MODE		   = 0x70, //工作模式
	SHORT_FRAME		   = 0x30, //短帧
	LONG_FRAME         = 0x60, //长帧
	GUI_FRAME		   = 0x33, //ZD
	TOM_FRAME          = 0x66, //TOM
	
	START_GUN		   = 0xa0, //发令枪
	SCAN			   = 0xa1, //扫描
	DISTANCE		   = 0xa2, //测距
	BEACON			   = 0xa3, //信令
};

enum FRAME_SUBTYPE
{
	START_GUN_REQ = 1,		//发令枪请求
	START_GUN_RES = 2,		//发令枪响应
	SCAN_REQ = 1,			//扫描请求
	SCAN_RES = 2,			//扫描响应
	SCAN_CON = 3,			//扫描确认
	DISTANCE_M = 1,			//测距M	
	DISTANCE_Z = 2,			//测距Z
};

enum TIMER_ID
{
	SCAN_REQ_TIMER = 5,		//扫描请求定时器
	SCAN_RES_TIMER,			//扫描响应定时器
	SCAN_CON_TIMER,			//扫描确认定时器
	Z_DATA_TIMER,		    //数据帧定时器Z
	START_GUN_TIMER			//发令枪定时器
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
	struct timespec base_time;	/*系统时钟下的基准时间*/
	uint64_t base_t;			/*PTP授时系统下的基准时间*/
	uint64_t start_time;		/*仿真开始时间*/
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
	pthread_t display_recv_thread_id;
	pthread_t display_thread_id;
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
	int my_role;
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
	//HANDLE hSharedMem;
	//LPVOID lpSharedMem;
	//FILE* data_file;
	//FILE* log_file;
	int mem_ptr;
	int act_prt;

	/*device_info*/
	device_info_t device_info;			//节点信息
	start_boardcast_t str;				//仿真开始时间
	int current_slot;					//当前时隙号
	int current_antenna;				//当前发射天线号
	int current_time_frame;				//当前时帧号
	int distance_flag_M[MAX_DEVICE];	//测距标志位M
	int scan_flag_M[MAX_DEVICE];		//扫描标志位M，置1发送扫描确认帧
	int scan_flag_Z;					//扫描标志位Z，置1发送扫描响应帧

#ifdef _WIN32
	UINT timerId;						//扫描询问、扫描回复定时器ID
	UINT timerId_Z;						//数据帧Z定时器ID
	UINT timerId_M[MAX_DEVICE];			//数据帧M定时器ID
#endif

	int antenna_M[MAX_DEVICE];			//天线匹配表M（0-5）
	int antenna_Z;						//天线匹配表Z（0-5）
	struct timespec set_network_st;		//系统时钟下建网开始时刻
	struct timespec set_network_ed;		//系统时钟下建网结束时刻
	uint64_t network_st;				//PTP授时系统下建网开始时刻
	uint64_t network_ed;				//PTP授时系统下建网结束时刻
	int set_network_time;				//建网时间
	int seq_m;							//数据包序列号M
	int seq_z;							//数据包序列号Z
	int seq_beacon_m;					//信令序列号M
	int seq_beacon_z;					//信令序列号Z
	int seq_distance_m;					//测距序列号M
	int seq_distance_z;					//测距序列号Z
	int time_schedule_flag;				//时隙调度标志位，置1表示发送时隙，置0表示接收时隙
	int time_frame_flag_m[MAX_DEVICE];	//时帧标志位M，指示Z建链完成后，M在这个时帧不发送数据，在下一时帧开始接收和发送数据
	int time_frame_flag_z;				//时帧标志位Z，指示Z建链完成后，在下一时帧发送数据帧给M
	//int chain_flag;
	int chain_flag_m;						//m建链标志位
	int chain_flag_z;                      //z建链标志位
	uint8_t  znode_connect_flag[MAX_DEVICE]; //mZ节点建链状态
	uint16_t m_proc_flight_control_data_tx_count;            // CANG终端向多任务处理QI发数据计数
	uint16_t m_proc_flight_control_data_rx_count;            // CANG终端向多任务处理QI收数据计数
	uint16_t m_z1_air_interface_data_tx_count;     //CANG终端向QI1终端发送空口业务数据包计数
	uint16_t m_z1_air_interface_data_rx_count;     //CANG终端向QI1终端接收空口业务数据包计数
	uint16_t z_proc_flight_control_data_tx_count; // 器向集成处理器飞控发数据计数
	uint16_t z_proc_flight_control_data_rx_count;  //器收集成处理器飞控数据计数
}info_t;

typedef struct NUM_CQ_AIR_DATA_TAG
{
	uint8_t  nodeId; //节点自身id
	uint16_t air_interface_data_tx_count;     //空口发送业务数据包计数
	uint16_t air_interface_data_rx_count;     //空口接收业务包计数
}NUM_CQ_AIR_DATA;

#pragma pack(1)
typedef struct _head_t
{
	uint8_t dst;				//目的地址	
	uint8_t src;				//源地址
	uint8_t type;				//包类型
	struct timespec send_time;	//系统时钟下发送时间
	uint64_t send_t;			//PTP授时系统下发送时间
	uint8_t antenna_id;			//天线ID
	int seq;					//发包序列号
	//位置信息
}head_t;

typedef struct _cp_t
{
	uint8_t para;
	/*待补充*/
}cp_t;

typedef struct _sp_t
{
	uint16_t st;				//自检结果
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
	head_t	head;				//包头
	uint8_t data[MAX_DATA_LEN]; //样机内部数据只传输data
	int		len;				//样机内部传输时不含head长度，样机间传输时包含head长度
}msg_t;
#pragma pack()

extern info_t info;
extern NUM_CQ_AIR_DATA g_node_progrm[MAX_DEVICE]; //节点管理数组
typedef struct 
{
	float x;
	float y;
	float z;
} Point3D;

typedef struct 
{
	float q0;
	float q1;
	float q2;
	float q3;
} Quaternion;

typedef struct 
{
	float dx;
	float dy;
	float dz;
	float h[3][3];
} AntennaTransform;

extern AntennaTransform transform[12];

#pragma pack(1)
typedef struct _fddi_info_t
{
	char ad;  //1
	int rti;  //4
	int tf;   //4
	int rtig; //4
	Point3D pos;
	Point3D v;
	Point3D rv;
	Quaternion q;
}fddi_info_t;
#pragma pack()


#pragma pack(1)
typedef struct 
{
	int len;
	int role;
	int index;		//选择天线
	int flag;
	Point3D pos;	//位置
	Point3D v;		//速度
	Point3D rv;		//角速度
	Quaternion q;	//四元数
	Point3D p_to; 
}psy_head_t;

typedef struct _psy_msg_t
{
	psy_head_t psy_head;
	msg_t msg;
}psy_msg_t;
#pragma pack()


extern fddi_info_t fddi_info;
extern fddi_info_t overall_fddi_info[5];
extern int8_t link_table[5][6];
extern int online_state[5];

void queue_init();

#endif
