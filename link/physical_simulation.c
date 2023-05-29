#include "physical_simulation.h"



void physical_send()
{
	/*crcУ��*/
	crc_check();
	/*���ݼ���*/
	data_scrambling();
	/*�ŵ�����*/
	channel_coding();
	/*�ŵ���֯*/
	channel_interleaving();
	/*���ݷ���*/
	data_clustering();
	/*��֡*/
	framing();
	/*MSK����*/
	msk_modulation();
	/*�ϱ�Ƶ*/
	up_conversion();
	/*�ŵ�����׼��*/

}

void physical_recv()
{
	/*�ŵ�����*/
	channel_simulation();
	/*�˲�*/
	filtering();
	/*ͬ��*/
	synchronous();
	/*���*/
	msk_demodulation();
	/*�⽻֯*/
	channel_deinterleaving();
	/*�ŵ�����*/
	channel_decode();
	/*���ݽ���*/
	data_descrambling();
	/*crcУ��*/
	crc_check();
}

/*crcУ��*/
void crc_check() {}

/*���ݼ���\����*/
void data_scrambling() {}
void data_descrambling() {}


/*�ŵ�����\����*/
void channel_coding() {}
void channel_decode() {}

/*�ŵ���֯\�⽻֯*/
void channel_interleaving() {}
void channel_deinterleaving() {}

/*���ݷ���*/
void data_clustering() {}

/*��֡*/
void framing() {}

/*MSK����\���*/
void msk_modulation() {}
void msk_demodulation() {}

/*�ϱ�Ƶ\�±�Ƶ*/
void up_conversion() {}
void down_conversion() {}

/*�˲�*/
void filtering() {}

/*ͬ��*/
void synchronous() {}


/* */

/*�ŵ�����*/
void channel_simulation() {}





