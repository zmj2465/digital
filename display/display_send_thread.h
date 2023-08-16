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

    SIM_START_ = 101,
    SIM_END_,
    REPLAY_SELECT_,
    REPLAY_START_,
    REPLAY_REP_,
    REPLAY_STOP_,
    REPLAY_RECOVER_,
    REPLAY_SPEED,
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
}display_state_t;

typedef struct 
{
    uint8_t tx_rx_status;       // �����շ�״̬ (0���ر� 1�����ڷ��� 2�����ڽ���)
    uint8_t beam_width;         // ���߲������
    double azimuth;             // ���߷�λ��
    double elevation;           // ���߸�����
    double eirp;                // ���ߵ�Чȫ����书��
    double gt;                  // ���߽�������
} antenna_t;

typedef struct 
{
    uint8_t node;               // �ڵ�
    double packet_loss_rate;    // ������
    double error_rate;          // ������
    double snr;                 // �����
    double received_signal_power;    // �����źŹ���
    double spreading_gain;      // ��Ƶ����
    double equivalent_spreading_factor; // ��Ч��Ƶ����
    double noise_level;         // ������ƽ
    double distance;            // ����
    double path_loss;           // ·�����
    double transmission_delay;  // ����ʱ��
    double doppler_shift;       // ������Ƶ��
    double radial_velocity;     // �����ٶ�
    double beam_angle;          // �����Ƕ�
    double antenna_gain;        // ��������
    double equivalent_isotropic_radiated_power;  // ��Чȫ����书��
    double transmitter_output_power; // ��������书��
    uint8_t state;
} channel_t;


typedef struct
{
    uint16_t serial_number;     // ���кţ�����ϴ��ͼ�¼��������ţ���0����
    struct timespec system_time;    // ϵͳʱ�䣬ǰ8���ֽڣ�long long��Ϊ��������4���ֽ�(long)Ϊ��������16�ֽڶ���
    float pos_x;                // ������λ��x
    float pos_y;                // ������λ��y
    float pos_z;                // ������λ��z
    float vel_x;                // �������ٶ�x
    float vel_y;                // �������ٶ�y
    float vel_z;                // �������ٶ�z
    float ang_vel_x;            // ��������̬���ٶ�x
    float ang_vel_y;            // ��������̬���ٶ�y
    float ang_vel_z;            // ��������̬���ٶ�z
    float quat_q0;              // ��������̬��Ԫ��q0
    float quat_q1;              // ��������̬��Ԫ��q1
    float quat_q2;              // ��������̬��Ԫ��q2
    float quat_q3;              // ��������̬��Ԫ��q3

    /*����*/
    uint16_t time_element_number;   // ʱԪ�ţ���ǰʱԪ�ţ�ÿ������1��0~65535��
    uint8_t time_frame_number;  // ʱ֡�ţ���ǰ֡�ţ�ÿ100ms����1��0~9��
    uint16_t micro_time_slot_number;    // ΢ʱ϶�ţ���ǰ΢ʱ϶�ţ�0~499��
    /*****/

    uint8_t node_role;          // �ڵ��ɫ��0��M�ڵ㣬1��Z�ڵ�
    uint8_t node_id;            // �ڵ�ID�ţ�0~4

    /*����*/
    uint8_t link_status;        // ��·����״̬��ռ��һ���ֽ�
    /*****/

    uint16_t z1_m_distance[4];     // Z1�ڵ���M�ڵ���룬ÿ�ڵ�2�ֽڣ�0~655.35km��
    uint16_t z1_m_azimuth[4];      // Z1�ڵ���M�ڵ���Է�λ��ÿ�ڵ�2�ֽ�
    uint16_t z1_m_elevation[4];    // Z1�ڵ���M�ڵ���Ը�����ÿ�ڵ�2�ֽ�

    /*����*/
    uint8_t comm_status_mode;   // ͨ��״̬/����ģʽ��1�ֽڣ�ɨ�衢������ͨ�š���Ĭ��С����/����
    uint32_t z_proc_flight_control_data_rx_tx_count;    // Z�뼯�ɴ������ɿ������շ�������Z�뼯�ɴ������ɿ����ݽ��ռ�����2�ֽڣ�Z�뼯�ɴ������ɿ����ݷ��ͼ�����2�ֽ�
    uint32_t z_proc_flight_control_data_rx_tx_timestamp;    // Z�뼯�ɴ������ɿ������շ�ʱ����Z�뼯�ɴ������ɿ����ݽ���ʱ����2�ֽڣ�Z�뼯�ɴ������ɿ����ݷ���ʱ����2�ֽ�
    uint32_t z_m_send_recv_count[4];
    uint8_t operation_status;   // ����״̬����Ƶ����Ƶ
    uint32_t channel_coding_decoding_frame_count;    // �ŵ�������֡�������ŵ�����֡������2�ֽڣ��ŵ�����֡������2�ֽ�
    uint32_t modulation_demodulation_frame_count;    // ���ƽ��֡����������֡������2�ֽڣ����֡������2�ֽ�
    uint32_t instruction_parsing_frame_count;    // ָ�����֡������4�ֽ�
    uint16_t m_node_time_freq_sync_status;      // M�ڵ�ʱƵͬ������״̬��M�ڵ�,ʱ��ͬ��״̬��1�ֽڣ�Ƶ��ͬ��״̬��1�ֽ�
    uint8_t m_node_downlink_link_status;    // M�ڵ�������·����״̬��M�ڵ㣬1�ֽ�
    uint16_t m_node_beam_azimuth_direction; // M�ڵ㲨����λָ��M�ڵ㣬2�ֽ�
    uint16_t m_node_beam_elevation_direction;   // M�ڵ㲨������ָ��M�ڵ㣬2�ֽ�
    uint8_t array_status[6];     // ���湤��״̬��6�����棬ÿ����1�ֽڣ�������խ�����������������桢���湤��״ָ̬ʾ
    uint8_t frequency_synthesizer_status;    // Ƶ��״̬��Ƶ�۹���״ָ̬ʾ
    uint32_t terminal_working_status_representation;   // �ն˹���״̬����
    antenna_t antenna_params[6];    // ���߲������������������Ӧ�����߻�������
    channel_t channel_params[4];    // �ŵ�������������4���ڵ�����ݴ�����ŵ���Ϣ
    /*****/

} display_t;


typedef struct {
    uint16_t seq;
    struct timespec system_time;
    uint8_t key;
    uint8_t node;
    float pos_x;                // ������λ��x
    float pos_y;                // ������λ��y
    float pos_z;                // ������λ��z
    float vel_x;                // �������ٶ�x
    float vel_y;                // �������ٶ�y
    float vel_z;                // �������ٶ�z
    float ang_vel_x;            // ��������̬���ٶ�x
    float ang_vel_y;            // ��������̬���ٶ�y
    float ang_vel_z;            // ��������̬���ٶ�z
    float quat_q0;              // ��������̬��Ԫ��q0
    float quat_q1;              // ��������̬��Ԫ��q1
    float quat_q2;              // ��������̬��Ԫ��q2
    float quat_q3;              // ��������̬��Ԫ��q3
}key_event_t;

typedef struct _show_t
{
    uint16_t type;
    uint16_t len;
    union {
        display_t display_info; //ң����Ϣ
        /*�������*/
        uint16_t  mode;          //���濪ʼ
        /*�������*/
        /*�ļ�����*/
        key_event_t key;        //�ؼ��¼�


        uint16_t file_seq;
        uint16_t data_seq;

        uint16_t file_num;
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

void generate_key_event(int type);


void sim_beg_proc(show_t* msg);
void sim_end_proc(show_t* msg);
void rep_sel_proc(show_t* msg);
void rep_beg_proc(show_t* msg);
void rep_rep_proc(show_t* msg);
void rep_suspend_proc(show_t* msg);
void rep_recover_proc(show_t* msg);

void send_display_msg();
void send_to_display(char* data, int len);
void find_data();

#endif