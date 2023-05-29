#include "physical_simulation.h"



void physical_send()
{
	/*crc校验*/
	crc_check();
	/*数据加扰*/
	data_scrambling();
	/*信道编码*/
	channel_coding();
	/*信道交织*/
	channel_interleaving();
	/*数据分组*/
	data_clustering();
	/*组帧*/
	framing();
	/*MSK调制*/
	msk_modulation();
	/*上变频*/
	up_conversion();
	/*信道仿真准备*/

}

void physical_recv()
{
	/*信道仿真*/
	channel_simulation();
	/*滤波*/
	filtering();
	/*同步*/
	synchronous();
	/*解调*/
	msk_demodulation();
	/*解交织*/
	channel_deinterleaving();
	/*信道译码*/
	channel_decode();
	/*数据解扰*/
	data_descrambling();
	/*crc校验*/
	crc_check();
}

/*crc校验*/
void crc_check() {}

/*数据加扰\解扰*/
void data_scrambling() {}
void data_descrambling() {}


/*信道编码\译码*/
void channel_coding() {}
void channel_decode() {}

/*信道交织\解交织*/
void channel_interleaving() {}
void channel_deinterleaving() {}

/*数据分组*/
void data_clustering() {}

/*组帧*/
void framing() {}

/*MSK调制\解调*/
void msk_modulation() {}
void msk_demodulation() {}

/*上变频\下变频*/
void up_conversion() {}
void down_conversion() {}

/*滤波*/
void filtering() {}

/*同步*/
void synchronous() {}


/* */

/*信道仿真*/
void channel_simulation() {}





