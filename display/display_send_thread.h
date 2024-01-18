#ifndef _DISPLAY_SEND_THREAD_H_
#define _DISPLAY_SEND_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include <dirent.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"

#define NO_MODE 0
#define SIM_MODE 1
#define REPLAY_MODE 2
#define OFF 0
#define ON 1

enum 
{
    DISPLAY_INFO,
    SIM_READY,
    SIM_START,
    SIM_END,
    FILE_SEQ,
    IMP_EVENT,
    ROLE_CONFIG=8,
    NET_NUM=10,

    SIM_START_ = 101,
    SIM_END_,
    REPLAY_SELECT_,
    REPLAY_START_,
    REPLAY_REP_,
    REPLAY_STOP_,
    REPLAY_RECOVER_,
    REPLAY_SPEED,
};


enum {
    KEY_POWER_ON,
    KEY_SELF_CHECK,
    KEY_CONFIG_LOAD,
    KEY_PRE_SEPARATE,
    KEY_SEPARATE,
    KEY_LINK_ON,
    KEY_LINK_OFF,
    KEY_RELINK_ON,
};

#pragma pack(1)

typedef struct
{
    int tx_flag;
    int mode;
    int interval;
    int seq;
    int find_seq;
    pthread_mutex_t mutex;
    FILE* file;
    int flag;
}display_state_t;



typedef struct
{
    uint16_t serial_number;     // 序列号，标记上传和记录的数据序号，从0递增
    struct timespec system_time;    // 系统时间，前8个字节（long long）为秒数，后4个字节(long)为纳秒数，16字节对齐
    float pos_x;                // 拦截器位置x
    float pos_y;                // 拦截器位置y
    float pos_z;                // 拦截器位置z
    float vel_x;                // 拦截器速度x
    float vel_y;                // 拦截器速度y
    float vel_z;                // 拦截器速度z
    float ang_vel_x;            // 拦截器姿态角速度x
    float ang_vel_y;            // 拦截器姿态角速度y
    float ang_vel_z;            // 拦截器姿态角速度z
    float quat_q0;              // 拦截器姿态四元数q0
    float quat_q1;              // 拦截器姿态四元数q1
    float quat_q2;              // 拦截器姿态四元数q2
    float quat_q3;              // 拦截器姿态四元数q3

    /*待定*/
    uint16_t time_element_number;   // 时元号，当前时元号，每秒增加1（0~65535）
    uint8_t time_frame_number;  // 时帧号，当前帧号，每100ms增加1（0~9）
    uint16_t micro_time_slot_number;    // 微时隙号，当前微时隙号（0~499）
    /*****/

    uint8_t node_role;          // 节点角色，0：M节点，1：Z节点
    uint8_t node_id;            // 节点ID号，0~4

    /*待定*/
    uint8_t link_status;        // 链路连接状态，占用一个字节
    /*****/

    float z1_m_distance[5];     // Z1节点与M节点距离，每节点2字节（0~655.35km）
    float z1_m_azimuth[5];      // Z1节点与M节点相对方位，每节点2字节
    float z1_m_elevation[5];    // Z1节点与M节点相对俯仰，每节点2字节


    /*待定*/
    uint8_t comm_status_mode;   // 通信状态/工作模式，1字节，扫描、建链、通信、静默、小功率/大功率
    uint32_t z_proc_flight_control_data_rx_tx_count;    // Z与集成处理器飞控数据收发计数，Z与集成处理器飞控数据接收计数：2字节，Z与集成处理器飞控数据发送计数：2字节
    uint32_t z_proc_flight_control_data_rx_tx_timestamp;    // Z与集成处理器飞控数据收发时戳，Z与集成处理器飞控数据接收时戳：2字节，Z与集成处理器飞控数据发送时戳：2字节
    uint32_t z_m_send_recv_count[4];
    uint8_t operation_status;   // 工作状态，定频、跳频
    uint32_t channel_coding_decoding_frame_count;    // 信道编译码帧计数，信道编码帧计数：2字节，信道译码帧计数：2字节
    uint32_t modulation_demodulation_frame_count;    // 调制解调帧计数，调制帧计数：2字节，解调帧计数：2字节
    uint32_t instruction_parsing_frame_count;    // 指令解析帧计数：4字节
    uint16_t m_node_time_freq_sync_status;      // M节点时频同步工作状态，M节点,时间同步状态：1字节，频率同步状态：1字节
    uint8_t m_node_downlink_link_status;    // M节点下行链路工作状态，M节点，1字节
    uint16_t m_node_beam_azimuth_direction; // M节点波束方位指向，M节点，2字节
    uint16_t m_node_beam_elevation_direction;   // M节点波束俯仰指向，M节点，2字节
    uint8_t array_status[6];     // 阵面工作状态，6个阵面，每阵面1字节，包含宽窄波束、波束所在阵面、阵面工作状态指示
    uint8_t frequency_synthesizer_status;    // 频综状态，频综工作状态指示
    uint32_t terminal_working_status_representation;   // 终端工作状态表征
    antenna_t antenna_params[6];    // 天线参数，与天线数量相对应的天线基本参数
    channel_t channel_params[4];    // 信道参数，与其它4个节点的数据传输的信道信息
    /*****/
    int8_t link_target[5][6];
    float  pitch[5];
    float  yaw[5];
    float  roll[5];

} display_t;




typedef struct {
    uint16_t seq;
    struct timespec system_time;
    uint8_t key;
    uint8_t role;
    uint8_t id;
    uint8_t m_num;
    uint8_t z_num;
    uint8_t target_role;
    uint8_t target_id;
    float pos_x;                // 拦截器位置x
    float pos_y;                // 拦截器位置y
    float pos_z;                // 拦截器位置z
    float vel_x;                // 拦截器速度x
    float vel_y;                // 拦截器速度y
    float vel_z;                // 拦截器速度z
    float ang_vel_x;            // 拦截器姿态角速度x
    float ang_vel_y;            // 拦截器姿态角速度y
    float ang_vel_z;            // 拦截器姿态角速度z
    float quat_q0;              // 拦截器姿态四元数q0
    float quat_q1;              // 拦截器姿态四元数q1
    float quat_q2;              // 拦截器姿态四元数q2
    float quat_q3;              // 拦截器姿态四元数q3
}key_event_t;


typedef struct _file_t
{
    uint16_t file_num;
    uint8_t  file_name[30][30];
}file_t;

typedef struct {
    uint8_t m_num;
    uint8_t z_num;
}net_t;

typedef struct {
    uint8_t role;
    uint8_t id;
}roleid_t;

typedef struct _show_t
{
    uint16_t type;
    uint16_t len;
    union {
        display_t display_info; //遥测信息
        /*仿真就绪*/
        uint16_t  mode;          //仿真开始
        /*仿真结束*/
        /*文件序列*/
        key_event_t key;        //关键事件

        uint16_t file_seq;
        uint16_t data_seq;

        file_t file_info;

        net_t num;
        roleid_t roleid;
    };
}show_t;

#pragma pack()

extern display_state_t display_state;
extern show_t show_msg;





static void init();

void* display_send_thread(void* arg);
void display_send_thread_init();

void select_file(show_t* msg);
void find_data();

void generate_key_event(int type, int id, int role);


void sim_beg_proc(show_t* msg);
void sim_end_proc(show_t* msg);
void rep_sel_proc(show_t* msg);
void rep_beg_proc(show_t* msg);
void rep_rep_proc(show_t* msg);
void rep_suspend_proc(show_t* msg);
void rep_recover_proc(show_t* msg);

//void send_display_msg();
void send_to_display(char* data, int len);
void find_data();

void role_id_config();

void set_zero(show_t* msg);



#endif