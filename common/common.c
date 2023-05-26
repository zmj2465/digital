#include "common.h"

info_t info;

void queue_init()
{
	int i;
	for (i = 0; i < END_THREAD; i++)
	{
		init_msg_queue(&info.thread_queue[i], 6);
	}




}