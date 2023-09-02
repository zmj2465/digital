#ifndef _RS485_RECV_THREAD_H_
#define _RS485_RECV_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"
#include "queue.h"

#define RS_MAX_LEN 1024


//body len
#define CONFIG_INFO_LEN 20
#define SHORT_FRAME_LEN sizeof(struct short_frame_t)-1 //65
#define SHORT_FRAME_SP_LEN sizeof(struct short_frame_sp_t) //66
#define LONG_FRAME_GUI_LEN    sizeof(struct long_frame_gui_t)-RS_TAIL_LEN //166=65+1+100
#define LONG_FRAME_TOM_LEN    sizeof(struct long_frame_tom_t)-RS_TAIL_LEN //588=65+1+522
#define LONG_FRAME_SP_GUI_LEN sizeof(struct long_frame_gui_sp_t)-RS_TAIL_LEN //153=66+87
#define LONG_FRAME_SP_TOM_LEN sizeof(struct long_frame_tom_sp_t)-RS_TAIL_LEN //582=66+516

//
#define RS_HEAD_LEN sizeof(rs_head_t)-(MY_INDEX==0?0:1)
#define RS_TAIL_LEN sizeof(rs_tail_t)


//total len
#define RS_SELF_CHECK_LEN RS_HEAD_LEN+RS_TAIL_LEN //13=7+6
#define RS_RESULT_LEN RS_HEAD_LEN+2+RS_TAIL_LEN //15=7+2+6
#define RS_CONFIG_LEN RS_HEAD_LEN+CONFIG_INFO_LEN+RS_TAIL_LEN //7++6
#define RS_SHORT_FRAME_SP_LEN RS_HEAD_LEN+SHORT_FRAME_SP_LEN+RS_TAIL_LEN //7++6

#define RS_LONG_FRAME_SP_GUI_LEN RS_HEAD_LEN+LONG_FRAME_SP_GUI_LEN+RS_TAIL_LEN //7++6
#define RS_LONG_FRAME_SP_TOM_LEN RS_HEAD_LEN+LONG_FRAME_SP_TOM_LEN+RS_TAIL_LEN //7++6

typedef struct _long_frame_buffer_t {
	char data[2048];
}long_frame_buffer_t;

extern long_frame_buffer_t m2z_data;
extern long_frame_buffer_t z2m_data;

#pragma pack(1)

enum {
	RS_SELF_CHECK=0x00,           //自检命令 
	RS_SLEF_CHECK_RESULT=0x10,    //自检结果下传命令
	RS_CONFIG_LOAD=0xCC,          //参数装订命令
	RS_START_LINK = 0x20,         //建链命令
	RS_SHORT_FRAME=0x30,          //短帧命令
	RS_LONG_FRAME=0x60,           //长帧命令
};


typedef struct _rs_head_t{
	uint8_t flag[4];
	uint8_t address_a;
	uint8_t address_b;
	uint8_t typea;
	uint8_t typeb;
}rs_head_t;

typedef struct _rs_tail_t {
	uint16_t crc;
	uint8_t flag[4];
}rs_tail_t;

struct result1_ack_t {
	uint16_t ack;
	rs_tail_t tail;
};

struct config_load_t {
	uint8_t node_role;
	uint8_t node_id;
	uint8_t node_num;
	uint8_t node_list;
	uint8_t freqC;
	uint8_t freqP;
	uint8_t reserve[13];
	rs_tail_t tail;
};


struct short_frame_t {
	uint8_t ad;
	uint32_t rti;
	int32_t tf;
	int32_t rtig;
	Point3D pos;
	Point3D v;
	Point3D rv;
	Quaternion q;
	uint8_t qzt;
	//rs_tail_t tail;
};

struct short_frame_sp_t {
	uint16_t time_element_number;        // 时元号
	uint8_t time_frame_number;           // 时帧号
	uint16_t micro_time_slot_number;     // 微时隙号
	uint8_t node_role;                  // 节点角色
	uint8_t node_id;                    // 节点ID号
	uint8_t link_status;                // 链路连接状态
	uint16_t z1_m_distance;          // 器节点与Cang节点距离
	int16_t z1_m_azimuth;            // 器节点与Cang节点相对方位
	int16_t z1_m_elevation;          // 器节点与Cang节点相对俯仰
	uint8_t comm_status_mode;           // 通信状态/工作模式
	uint16_t z_proc_flight_control_data_tx_count; // 器向集成处理器飞控发数据计数
	uint16_t z_proc_flight_control_data_rx_count;  //器收集成处理器飞控数据计数
	uint32_t z_proc_flight_control_data_tx_timestamp;   // 器向集成处理器飞控发数据时戳
	uint32_t z_proc_flight_control_data_rx_timestamp;   // 器收集成处理器飞控数据时戳
	uint16_t z1_m_air_interface_data_tx_count;     //器1/器2/器3/器4向Cang发送空口业务数据包计数
	uint16_t z1_m_air_interface_data_rx_count;     //器1/器2/器3/器4收Cang空口业务包计数
	uint16_t channel_coding_frame_count;       //信道编码帧计数
	uint16_t channel_decoding_frame_count;     //信道译码帧计数
	uint16_t modulation_frame_count;           //调制帧计数
	uint16_t demodulation_frame_count;         //解调帧计数
	uint32_t instruction_parsing_frame_count;            // 指令解析帧计数
	uint8_t array_status;                // 阵面工作状态
	uint8_t instruction_crc_error_count; // 指令CRC错误计数
	uint8_t address_error_count;         // 地址码错误计数
	uint8_t air_packet_loss_count;       // 空口接收丢包计数
	uint32_t terminal_working_status_representation; // 终端工作状态表征
	uint8_t  reserved[15];               // 保留字段
	//rs_tail_t tail;
};

typedef struct {
	// 参数1~7：LJQ导航信息（位置和速度）
	uint32_t LJQ_data_UTC;          // 4字节，LJQ数据的UTC时刻，单位0.1ms
	float LJQ_nav_position_X;       // 4字节，X坐标位置，单位米
	float LJQ_nav_position_Y;       // 4字节，Y坐标位置，单位米
	float LJQ_nav_position_Z;       // 4字节，Z坐标位置，单位米
	float LJQ_nav_velocity_Vx;      // 4字节，X轴方向速度，单位米/秒
	float LJQ_nav_velocity_Vy;      // 4字节，Y轴方向速度，单位米/秒
	float LJQ_nav_velocity_Vz;      // 4字节，Z轴方向速度，单位米/秒

	// 参数8~16：时刻和节点信息
	uint16_t time_epoch_number;      // 2字节，时元号，每秒增加1
	uint8_t time_frame_number;       // 1字节，当前帧号，每100ms增加1
	uint16_t time_micro_slot_number; // 2字节，当前微时隙号，每0.2ms增加1
	uint8_t node_role;               // 1字节，节点角色 (0: Cang节点, 1: 器节点)
	uint8_t node_id;                 // 1字节，节点ID号
	uint8_t link_status;             // 1字节，链路连接状态

	// 参数17~27：通信统计信息
	uint8_t communication_mode;          // 1字节，通信状态/工作模式
	uint16_t Cang_to_CentralUnit_tx_count;   // 2字节，Cang向中央处理单元发数据计数
	uint16_t Cang_from_CentralUnit_rx_count; // 2字节，Cang收中央处理单元数据计数
	uint32_t Cang_to_CentralUnit_tx_timestamp;   // 4字节，Cang向中央处理单元发数时戳（UTC时间，单位0.1ms）
	uint32_t Cang_from_CentralUnit_rx_timestamp; // 4字节，Cang收中央处理单元数据时戳（UTC时间，单位0.1ms）

	// 参数28~32：空口业务包统计信息
	struct {
		uint16_t Cang_to_Device_tx_count; // 2字节，Cang向器1发送空口业务数据包计数
		uint16_t Cang_from_Device_rx_count // 2字节，Cang收器1空口业务包计数
	}rx_tx_count[4];

	// 参数33~43：节点和Cang的相对信息（距离、波束方向、波束俯仰）
	struct {
		uint16_t distance;          // 2字节，距离，单位0.01km
		int16_t beam_azimuth;       // 2字节，波束方位指向，单位0.01°
		int16_t beam_elevation;     // 2字节，波束俯仰指向，单位0.01°
	} node_distances[4];            // 4组，每组包含3个参数，对应4个节点

	// 参数44~49：状态和计数信息
	uint8_t array_working_status;      // 1字节，阵面工作状态
	uint8_t command_crc_error_count;   // 1字节，指令CRC错误计数
	uint8_t address_error_count;       // 1字节，地址码错误计数
	uint32_t air_interface_packet_loss_count; // 4字节，空口接收丢包计数（对于Cang节点）

	// 参数50~57：终端工作状态和保留字段
	uint32_t terminal_working_status;  // 4字节，终端工作状态表征
	uint32_t reserved[2];             // 保留的8字节

} LJQ_parameters;


struct long_frame_gui_t {
	struct short_frame_t short_frame;
	
	uint32_t framec;
	uint8_t typec;
	uint8_t guide_qcdata[45];
	uint32_t frameq;
	uint8_t typeq;
	uint8_t x2y;
	uint8_t qqdata[44];

	rs_tail_t tail;
};

struct long_frame_tom_t {
	struct short_frame_t short_frame;

	uint32_t framec;
	uint8_t typec;
	uint8_t tom_qcdata[467];
	uint32_t frameq;
	uint8_t typeq;
	uint8_t x2y;
	uint8_t qqdata[44];

	rs_tail_t tail;
};

struct long_frame_gui_sp_t {
	struct short_frame_sp_t short_frame_sp;

	uint32_t framec;
	uint8_t typec;
	uint8_t guide_cqdata[32];
	uint32_t frameq;
	uint8_t typeq;
	uint8_t x2y;
	uint8_t qqdata[44];

	rs_tail_t tail;
};

struct long_frame_tom_sp_t {
	struct short_frame_sp_t short_frame_sp;

	uint32_t framec;
	uint8_t typec;
	uint8_t tom_cqdata[455];
	uint32_t frameq;
	uint8_t typeq;
	uint8_t x2y;
	uint8_t qqdata[44];

	rs_tail_t tail;
};

typedef union _rs_body_t {
	rs_tail_t body_tail;
	struct result1_ack_t result1_ack;
	struct config_load_t config_load;

	struct short_frame_t short_frmae;
	struct short_frame_sp_t short_frmae_sp;

	struct long_frame_gui_t long_frame_gui;
	struct long_frame_tom_t long_frame_tom;
	struct long_frame_gui_sp_t long_frame_sp_gui;
	struct long_frame_tom_sp_t long_frame_sp_tom;
}rs_body_t;



#pragma pack()






void* rs_485_recv_thread(void* arg);


void rs_SelfCheck_proc(char* data);
void rs_SelfCheckResult_proc(char* data);
void rs_ConfigLoad_proc(char* data);
void rs_Link_proc(char* data);
void rs_ShortFrame_proc(char* data);
void rs_LongFrame_proc(char* data, int len);

void head_load(char* data, char* res);
uint8_t crc_check(char* start_address, int len, uint16_t get_crc);



#endif
