#include "display_thread.h"



void* display_thread(void* arg)
{
	pthread_detach(pthread_self());


	while (1)
	{
		find_data();
		Sleep(3000);
	}
}