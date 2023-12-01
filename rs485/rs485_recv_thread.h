#ifndef _RS485_RECV_THREAD_H_
#define _RS485_RECV_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"
#include "queue.h"

#define RS_MAX_LEN 2048

//addressa addressb type
#define ADD_TYPE_LEN (3+(MY_INDEX==0?1:0))

//body len
#define CONFIG_INFO_LEN 20
#define LINK_RESULT_SP_LEN sizeof(struct link_result_sp_t)-RS_TAIL_LEN

#define Z_SHORT_FRAME_LEN sizeof(struct z_short_frame_t) //66
#define Z_SHORT_FRAME_SP_LEN sizeof(struct z_short_frame_sp_t) 

#define M_SHORT_FRAME_LEN sizeof(struct m_short_frame_t) //66
#define M_SHORT_FRAME_SP_LEN sizeof(struct m_short_frame_sp_t) 

#define LONG_FRAME_GUI_LEN    sizeof(struct long_frame_gui_t)-RS_TAIL_LEN //166=65+1+100
#define LONG_FRAME_TOM_LEN    sizeof(struct long_frame_tom_t)-RS_TAIL_LEN //588=65+1+522
#define LONG_FRAME_SP_GUI_LEN sizeof(struct long_frame_gui_sp_t)-RS_TAIL_LEN //153=66+87
#define LONG_FRAME_SP_TOM_LEN sizeof(struct long_frame_tom_sp_t)-RS_TAIL_LEN //582=66+516

#define M2Z_GUI_FRAME_LEN sizeof(struct m2z_gui_frame_t)-RS_TAIL_LEN
#define M2Z_TOM_FRAME_LEN sizeof(struct m2z_tom_frame_t)-RS_TAIL_LEN
#define M2Z_PLAN_FRAME_LEN sizeof(struct m2z_plan_frame_t)-RS_TAIL_LEN

#define M2Z_GUI_FRAME_SP_LEN sizeof(struct m2z_gui_frame_sp_t)-RS_TAIL_LEN
#define M2Z_TOM_FRAME_SP_LEN sizeof(struct m2z_tom_frame_sp_t)-RS_TAIL_LEN
#define M2Z_PLAN_FRAME_SP_LEN sizeof(struct m2z_plan_frame_sp_t)-RS_TAIL_LEN

//struct m2z_gui_frame_t m2z_gui_frame;
//struct m2z_gui_frame_sp_t m2z_gui_frame_sp;
//struct m2z_tom_frame_t m2z_tom_frame;
//struct m2z_tom_frame_sp_t m2z_tom_frame_sp;
//struct m2z_plan_frame_t m2z_plan_frame;
//struct m2z_plan_frame_sp_t m2z_plan_frame_sp;

//
#define RS_HEAD_LEN sizeof(rs_head_t)-(MY_INDEX==0?0:1)
#define RS_TAIL_LEN sizeof(rs_tail_t)


//total len
#define RS_SELF_CHECK_LEN RS_HEAD_LEN+RS_TAIL_LEN //13=7+6
#define RS_RESULT_LEN RS_HEAD_LEN+2+RS_TAIL_LEN //15=7+2+6
#define RS_CONFIG_LEN RS_HEAD_LEN+CONFIG_INFO_LEN+RS_TAIL_LEN //7++6

#define RS_LINK_START_LEN RS_HEAD_LEN+RS_TAIL_LEN
#define RS_LINK_RESULT_SP_LEN RS_HEAD_LEN+LINK_RESULT_SP_LEN+RS_TAIL_LEN
#define RS_WORK_MODE_SP_LEN RS_HEAD_LEN+1+RS_TAIL_LEN
#define RS_PRE_SEPARATE_SP_LEN RS_HEAD_LEN+RS_TAIL_LEN


#define RS_Z_SHORT_FRAME_SP_LEN RS_HEAD_LEN+Z_SHORT_FRAME_SP_LEN+RS_TAIL_LEN //7++6
#define RS_M_SHORT_FRAME_SP_LEN RS_HEAD_LEN+M_SHORT_FRAME_SP_LEN+RS_TAIL_LEN //7++6


#define RS_LONG_FRAME_GUI_LEN RS_HEAD_LEN+LONG_FRAME_GUI_LEN+RS_TAIL_LEN //7++6
#define RS_LONG_FRAME_TOM_LEN RS_HEAD_LEN+LONG_FRAME_TOM_LEN+RS_TAIL_LEN //7++6
#define RS_LONG_FRAME_SP_GUI_LEN RS_HEAD_LEN+LONG_FRAME_SP_GUI_LEN+RS_TAIL_LEN //7++6
#define RS_LONG_FRAME_SP_TOM_LEN RS_HEAD_LEN+LONG_FRAME_SP_TOM_LEN+RS_TAIL_LEN //7++6


#define RS_M2Z_GUI_FRAME_LEN RS_HEAD_LEN+M2Z_GUI_FRAME_LEN+RS_TAIL_LEN
#define RS_M2Z_TOM_FRAME_LEN RS_HEAD_LEN+M2Z_TOM_FRAME_LEN+RS_TAIL_LEN
#define RS_M2Z_PLAN_FRAME_LEN RS_HEAD_LEN+M2Z_PLAN_FRAME_LEN+RS_TAIL_LEN

#define RS_M2Z_GUI_FRAME_SP_LEN RS_HEAD_LEN+M2Z_GUI_FRAME_SP_LEN+RS_TAIL_LEN
#define RS_M2Z_TOM_FRAME_SP_LEN RS_HEAD_LEN+M2Z_TOM_FRAME_SP_LEN+RS_TAIL_LEN
#define RS_M2Z_PLAN_FRAME_SP_LEN RS_HEAD_LEN+M2Z_PLAN_FRAME_SP_LEN+RS_TAIL_LEN

typedef struct _long_frame_buffer_t {
	char data[2048];
}long_frame_buffer_t;

extern long_frame_buffer_t m2z_data;
extern long_frame_buffer_t z2m_data;

#pragma pack(1)

enum {
	RS_SELF_CHECK=0x00,           //�Լ����� 
	RS_SLEF_CHECK_RESULT=0x10,    //�Լ����´�����
	RS_CONFIG_LOAD=0xCC,          //����װ������
	RS_WORK_MODE=0x70,            //����ģʽ����
	RS_START_LINK = 0x20,         //��������
	RS_LINK_RESULT = 0x40,        //�������
	RS_PRE_SEPARATE = 0xDD,       //Ԥ����
	RS_SHORT_FRAME=0x30,          //��֡����
	RS_LONG_FRAME=0x60,           //��֡����
};

enum {
	RS_M2Z_GUI = 0x0100,
	RS_M2Z_TOM = 0x0200,
	RS_M2Z_PLAN = 0x0300,
	RS_Z2M_GUI_SP = 0x1A00,
	RS_Z2M_TOM_SP = 0x1B00,
	RS_Z2M_PLAN_SP = 0x1C00,
};


typedef struct _rs_head_t{
	uint8_t flag[4];
	uint8_t address_a;
	uint8_t address_b;
	union {
		uint8_t typea;
		uint16_t type;
	};
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
	uint16_t freq_fix;
	uint8_t reserve[12];
	rs_tail_t tail;
};


struct work_mode_t {
	uint8_t address;
	uint8_t mode;
	rs_tail_t tail;
};

struct work_mode_sp_t {
	uint8_t config_result;
	rs_tail_t tail;
};

struct link_result_sp_t {
	uint8_t  link_state;
	uint16_t switch_num;
	rs_tail_t tail;
};


struct z_short_frame_t {
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

struct z_short_frame_sp_t {
	uint16_t time_element_number;        // ʱԪ��
	uint8_t time_frame_number;           // ʱ֡��
	uint16_t micro_time_slot_number;     // ΢ʱ϶��
	uint8_t node_id;                    // �ڵ�ID��

	//uint8_t table[71];
	uint8_t link_status;                // ��·����״̬
	uint16_t z1_m_distance;          // ���ڵ���Cang�ڵ����
	int16_t z1_m_azimuth;            // ���ڵ���Cang�ڵ���Է�λ
	int16_t z1_m_elevation;          // ���ڵ���Cang�ڵ���Ը���
	uint8_t comm_status_mode;           // ͨ��״̬/����ģʽ
	uint16_t z_proc_flight_control_data_tx_count; // ���򼯳ɴ������ɿط����ݼ���
	uint16_t z_proc_flight_control_data_rx_count;  //���ռ��ɴ������ɿ����ݼ���
	uint32_t z_proc_flight_control_data_tx_timestamp;   // ���򼯳ɴ������ɿط�����ʱ��
	uint32_t z_proc_flight_control_data_rx_timestamp;   // ���ռ��ɴ������ɿ�����ʱ��
	uint16_t z1_m_air_interface_data_tx_count;     //��1/��2/��3/��4��Cang���Ϳտ�ҵ�����ݰ�����
	uint16_t z1_m_air_interface_data_rx_count;     //��1/��2/��3/��4��Cang�տ�ҵ�������
	uint16_t channel_coding_frame_count;       //�ŵ�����֡����
	uint16_t channel_decoding_frame_count;     //�ŵ�����֡����
	uint16_t modulation_frame_count;           //����֡����
	uint16_t demodulation_frame_count;         //���֡����
	uint16_t instruction_parsing_frame_count;            // ָ�����֡����
	uint8_t array_status;                // ���湤��״̬
	uint8_t instruction_crc_error_count; // ָ��CRC�������
	uint8_t address_error_count;         // ��ַ��������
	uint8_t air_packet_loss_count;       // �տڽ��ն�������

	uint16_t recv_error_count;           //���������쳣����
	uint16_t send_error_count;           //��������ʧ�ܼ���
	uint8_t  antenna_selection_state;    //����ѡ��״̬
	uint8_t  antenna_txrx_state;         //�����շ�״̬
	uint8_t  beam_width;                 //�������

	//uint32_t terminal_working_status_representation; // �ն˹���״̬����
	uint8_t  reserved[26];               // ״̬�֣�5��+ �����ֶΣ�26��
	//rs_tail_t tail;
};

struct m_short_frame_t {
	uint8_t en;
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

struct m_short_frame_sp_t {

	//uint8_t table[158];
	uint32_t time_ljqi;
	float     posX;
	float     posY;
	float     posZ;
	float     Vx;
	float     Vy;
	float     Vz;
	uint16_t time_element_number;        // ʱԪ��
	uint8_t time_frame_number;           // ʱ֡��
	uint16_t micro_time_slot_number;     // ΢ʱ϶��
	uint8_t node_id;                    // �ڵ�ID��

	uint8_t link_status;                // ��·����״̬
	uint8_t comm_status_mode;           // ͨ��״̬/����ģʽ
	uint16_t m_proc_flight_control_data_tx_count;            // CANG�ն����������QI�����ݼ���
	uint16_t m_proc_flight_control_data_rx_count;            // CANG�ն����������QI�����ݼ���
	uint32_t m_proc_flight_control_data_tx_timestamp;   // cang�򼯳ɴ������ɿط�����ʱ��
	uint32_t m_proc_flight_control_data_rx_timestamp;   // cang�ռ��ɴ������ɿ�����ʱ��
	uint16_t m_z1_air_interface_data_tx_count;     //CANG�ն���QI1�ն˷��Ϳտ�ҵ�����ݰ�����
	uint16_t m_z1_air_interface_data_rx_count;     //CANG�ն���QI1�ն˽��տտ�ҵ�����ݰ�����
	uint16_t m_z2_air_interface_data_tx_count;     //CANG�ն���QI2�ն˷��Ϳտ�ҵ�����ݰ�����
	uint16_t m_z2_air_interface_data_rx_count;     //CANG�ն���QI2�ն˽��տտ�ҵ�����ݰ�����
	uint16_t m_z3_air_interface_data_tx_count;     //CANG�ն���QI3�ն˷��Ϳտ�ҵ�����ݰ�����
	uint16_t m_z3_air_interface_data_rx_count;     //CANG�ն���QI3�ն˽��տտ�ҵ�����ݰ�����
	uint16_t m_z4_air_interface_data_tx_count;     //CANG�ն���QI4�ն˷��Ϳտ�ҵ�����ݰ�����
	uint16_t m_z4_air_interface_data_rx_count;     //CANG�ն���QI4�ն˽��տտ�ҵ�����ݰ�����
	uint16_t channel_coding_frame_count;       //�ŵ�����֡����
	uint16_t channel_decoding_frame_count;     //�ŵ�����֡����
	uint16_t modulation_frame_count;           //����֡����
	uint16_t demodulation_frame_count;         //���֡����
	uint32_t instruction_parsing_frame_count;            // ָ�����֡����
	uint16_t m_z1_distance;                    //Cang��Z1����
	int16_t  m_z1_fangwei;                    //CANG�ն���QI1�ն˲�����λָ��
	uint16_t m_z1_fuyang;                     //CANG�ն���QI1�ն˲�������ָ��
	uint16_t m_z2_distance;                    //Cang��Z2����
	int16_t  m_z2_fangwei;                    //CANG�ն���QI2�ն˲�����λָ��
	uint16_t m_z2_fuyang;                     //CANG�ն���QI2�ն˲�������ָ��
	uint16_t m_z3_distance;                    //Cang��Z3����
	int16_t  m_z3_fangwei;                    //CANG�ն���QI3�ն˲�����λָ��
	uint16_t m_z3_fuyang;                     //CANG�ն���QI3�ն˲�������ָ��
	uint16_t m_z4_distance;                    //Cang��Z4����
	int16_t  m_z4_fangwei;                    //CANG�ն���QI4�ն˲�����λָ��
	uint16_t m_z4_fuyang;                     //CANG�ն���QI4�ն˲�������ָ��
	uint8_t array_status;                // ���湤��״̬
	uint8_t instruction_crc_error_count; // ָ��CRC�������
	uint8_t address_error_count;         // ��ַ��������
	uint32_t air_packet_loss_count;       // �տڽ��ն�������
	//uint32_t terminal_working_status_representation; // �ն˹���״̬����
	uint8_t  reserved[51];               // �����ֶ�
	//rs_tail_t tail;
};

typedef struct {
	// ����1~7��LJQ������Ϣ��λ�ú��ٶȣ�
	uint32_t LJQ_data_UTC;          // 4�ֽڣ�LJQ���ݵ�UTCʱ�̣���λ0.1ms
	float LJQ_nav_position_X;       // 4�ֽڣ�X����λ�ã���λ��
	float LJQ_nav_position_Y;       // 4�ֽڣ�Y����λ�ã���λ��
	float LJQ_nav_position_Z;       // 4�ֽڣ�Z����λ�ã���λ��
	float LJQ_nav_velocity_Vx;      // 4�ֽڣ�X�᷽���ٶȣ���λ��/��
	float LJQ_nav_velocity_Vy;      // 4�ֽڣ�Y�᷽���ٶȣ���λ��/��
	float LJQ_nav_velocity_Vz;      // 4�ֽڣ�Z�᷽���ٶȣ���λ��/��

	// ����8~16��ʱ�̺ͽڵ���Ϣ
	uint16_t time_epoch_number;      // 2�ֽڣ�ʱԪ�ţ�ÿ������1
	uint8_t time_frame_number;       // 1�ֽڣ���ǰ֡�ţ�ÿ100ms����1
	uint16_t time_micro_slot_number; // 2�ֽڣ���ǰ΢ʱ϶�ţ�ÿ0.2ms����1
	uint8_t node_role;               // 1�ֽڣ��ڵ��ɫ (0: Cang�ڵ�, 1: ���ڵ�)
	uint8_t node_id;                 // 1�ֽڣ��ڵ�ID��
	uint8_t link_status;             // 1�ֽڣ���·����״̬

	// ����17~27��ͨ��ͳ����Ϣ
	uint8_t communication_mode;          // 1�ֽڣ�ͨ��״̬/����ģʽ
	uint16_t Cang_to_CentralUnit_tx_count;   // 2�ֽڣ�Cang�����봦��Ԫ�����ݼ���
	uint16_t Cang_from_CentralUnit_rx_count; // 2�ֽڣ�Cang�����봦��Ԫ���ݼ���
	uint32_t Cang_to_CentralUnit_tx_timestamp;   // 4�ֽڣ�Cang�����봦��Ԫ����ʱ����UTCʱ�䣬��λ0.1ms��
	uint32_t Cang_from_CentralUnit_rx_timestamp; // 4�ֽڣ�Cang�����봦��Ԫ����ʱ����UTCʱ�䣬��λ0.1ms��

	// ����28~32���տ�ҵ���ͳ����Ϣ
	struct {
		uint16_t Cang_to_Device_tx_count; // 2�ֽڣ�Cang����1���Ϳտ�ҵ�����ݰ�����
		uint16_t Cang_from_Device_rx_count // 2�ֽڣ�Cang����1�տ�ҵ�������
	}rx_tx_count[4];

	// ����33~43���ڵ��Cang�������Ϣ�����롢�������򡢲���������
	struct {
		uint16_t distance;          // 2�ֽڣ����룬��λ0.01km
		int16_t beam_azimuth;       // 2�ֽڣ�������λָ�򣬵�λ0.01��
		int16_t beam_elevation;     // 2�ֽڣ���������ָ�򣬵�λ0.01��
	} node_distances[4];            // 4�飬ÿ�����3����������Ӧ4���ڵ�

	// ����44~49��״̬�ͼ�����Ϣ
	uint8_t array_working_status;      // 1�ֽڣ����湤��״̬
	uint8_t command_crc_error_count;   // 1�ֽڣ�ָ��CRC�������
	uint8_t address_error_count;       // 1�ֽڣ���ַ��������
	uint32_t air_interface_packet_loss_count; // 4�ֽڣ��տڽ��ն�������������Cang�ڵ㣩

	// ����50~57���ն˹���״̬�ͱ����ֶ�
	uint32_t terminal_working_status;  // 4�ֽڣ��ն˹���״̬����
	uint32_t reserved[2];             // ������8�ֽ�

} LJQ_parameters;


struct long_frame_gui_t {
	struct z_short_frame_t z_short_frame;
	
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
	struct z_short_frame_t z_short_frame;

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
	struct z_short_frame_sp_t z_short_frame_sp;

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
	struct z_short_frame_sp_t z_short_frame_sp;

	uint32_t framec;
	uint8_t typec;
	uint8_t tom_cqdata[455];
	uint32_t frameq;
	uint8_t typeq;
	uint8_t x2y;
	uint8_t qqdata[44];

	rs_tail_t tail;
};

struct m2z_gui_frame_t {

	uint8_t content[132];

	rs_tail_t tail;
};

struct m2z_gui_frame_sp_t {

	uint8_t content[184];

	rs_tail_t tail;
};

struct m2z_tom_frame_t {

	uint8_t content[1824];

	rs_tail_t tail;
};

struct m2z_tom_frame_sp_t {

	uint8_t content[1872];

	rs_tail_t tail;
};

struct m2z_plan_frame_t {

	uint8_t content[376];

	rs_tail_t tail;
};

struct m2z_plan_frame_sp_t {

	uint8_t content[188];

	rs_tail_t tail;
};


typedef union _rs_body_t {
	rs_tail_t body_tail;
	struct result1_ack_t result1_ack; //�Լ����ظ�
	struct config_load_t config_load; //װ�������ظ�

	struct z_short_frame_t z_short_frmae; //��֡
	struct z_short_frame_sp_t z_short_frmae_sp; //��֡�ظ�

	struct m_short_frame_t m_short_frmae; //��֡
	struct m_short_frame_sp_t m_short_frmae_sp; //��֡�ظ�

	struct long_frame_gui_t long_frame_gui; //��֡����
	struct long_frame_tom_t long_frame_tom; //��֡tom
	struct long_frame_gui_sp_t long_frame_sp_gui; //��֡�����ظ�
	struct long_frame_tom_sp_t long_frame_sp_tom; //��֡tom�ظ�

	struct m2z_gui_frame_t m2z_gui_frame; //m2z����
	struct m2z_gui_frame_sp_t m2z_gui_frame_sp; //m2z�����ظ�
	struct m2z_tom_frame_t m2z_tom_frame; //m2z tom
	struct m2z_tom_frame_sp_t m2z_tom_frame_sp; //m2z tom �ظ�
	struct m2z_plan_frame_t m2z_plan_frame; //m2zͨ�żƻ�
	struct m2z_plan_frame_sp_t m2z_plan_frame_sp; //m2zͨ�żƻ��ظ�

	struct work_mode_t work_mode; //����ģʽ
	struct work_mode_sp_t work_modes_sp; //����ģʽ�ظ�

	struct link_result_sp_t link_result_sp; //��������ظ�


}rs_body_t;


#pragma pack()



int rs485_init();
void server_init();








void* rs_485_recv_thread(void* arg);


void rs_SelfCheck_proc(char* data);
void rs_SelfCheckResult_proc(char* data);
void rs_ConfigLoad_proc(char* data);
void work_mode_proc(char* data);
void rs_Link_proc(char* data);
void rs_Link_result_proc(char* data);
void rs_PreSeparate_proc(char* data);


void rs_ShortFrame_proc(char* data);
void rs_LongFrame_proc(char* data, int len);

void rs_M2ZGui_proc(char* data);
void rs_M2ZTom_proc(char* data);
void rs_M2ZPlan_proc(char* data);


void create_CheckResult_res();
void create_ConfigLoad_res();
void create_WorkMode_res();
void create_LinkResult_res();
void create_ShortFrame_res();

void head_load(char* data, char* res);
void head_load_b(char* res);
uint8_t crc_check(char* start_address, int len, uint16_t get_crc);



#endif
