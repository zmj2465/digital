#ifndef _PHYSICAL_SIMULATION_H_
#define _PHYSICAL_SIMULATION_H_

#include "common.h"
#include <stdbool.h>
#include "angle.h"


#define MAX_DISTANCE 500000
extern int prepare_simulation;
typedef bool (*phy_fun)(psy_msg_t*);

struct antenna_info_t {
    Point3D point_to;
};

extern struct antenna_info_t antenna_info[6];


/*crc校验*/
void crc_check();

/*数据加扰\解扰*/
void data_scrambling();
void data_descrambling();


/*信道编码\译码*/
void channel_coding();
void channel_decode();

/*信道交织\解交织*/
void channel_interleaving();
void channel_deinterleaving();

/*数据分组*/
void data_clustering();

/*组帧*/
void framing();

/*MSK调制\解调*/
void msk_modulation();
void msk_demodulation();

/*上变频\下变频*/
void up_conversion();
void down_conversion();

/*滤波*/
void filtering();

/*同步*/
void synchronous();

/* */


void channel_simulation();

int psy_recv(int len, char* data, msg_t* msg, int index, int role);
void psy_send(int len, char* data, msg_t* msg, int index, int role);

bool psy_recv_(psy_msg_t* data, msg_t* msg);
int psy_send_(psy_msg_t* data, msg_t* msg);

bool config_judge(psy_msg_t* p);
bool distance_judge(psy_msg_t* p);
bool antenna_match_(psy_msg_t* p);
bool channel_sim(psy_msg_t* data);

bool new_angle_check(Point3D send_p, Point3D recv_p, Quaternion send_q, Quaternion recv_q, int send_r, int recv_r, int send_i, int recv_i);

#endif