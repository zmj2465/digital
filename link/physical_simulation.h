#ifndef _PHYSICAL_SIMULATION_H_
#define _PHYSICAL_SIMULATION_H_

#include "common.h"
#include <stdbool.h>
#include "angle.h"

extern int prepare_simulation;

/*crcУ��*/
void crc_check();

/*���ݼ���\����*/
void data_scrambling();
void data_descrambling();


/*�ŵ�����\����*/
void channel_coding();
void channel_decode();

/*�ŵ���֯\�⽻֯*/
void channel_interleaving();
void channel_deinterleaving();

/*���ݷ���*/
void data_clustering();

/*��֡*/
void framing();

/*MSK����\���*/
void msk_modulation();
void msk_demodulation();

/*�ϱ�Ƶ\�±�Ƶ*/
void up_conversion();
void down_conversion();

/*�˲�*/
void filtering();

/*ͬ��*/
void synchronous();

/* */


void channel_simulation();

int psy_recv(int len, char* data, char* msg, int index, int role);
void psy_send(int len, char* data, char* msg, int index, int role);

#endif