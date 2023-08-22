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


bool psy_recv_(psy_msg_t* data, msg_t* msg);
int psy_send_(psy_msg_t* data, msg_t* msg);

bool config_judge(psy_msg_t* p);
bool distance_judge(psy_msg_t* p);
bool antenna_match_(psy_msg_t* p);
bool channel_sim(psy_msg_t* data);

bool new_angle_check(Point3D send_p, Point3D recv_p, Quaternion send_q, Quaternion recv_q, int send_r, int recv_r, int send_i, int recv_i);

#endif