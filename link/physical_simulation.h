#ifndef _PHYSICAL_SIMULATION_H_
#define _PHYSICAL_SIMULATION_H_

#include "common.h"
#include <stdbool.h>
#include "angle.h"

extern int prepare_simulation;

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

int psy_recv(int len, char* data, char* msg, int index, int role);
void psy_send(int len, char* data, char* msg, int index, int role);

#endif