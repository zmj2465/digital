#include "display_thread.h"
#include "display_send_thread.h"


void* display_thread(void* arg)
{
	pthread_detach(pthread_self());
	int x = 0;

	while (1)
	{
		if (display_state.mode == SIM_MODE)
		{
			send_display_msg();
			Sleep(display_state.interval);
		}
		else if (display_state.mode == REPLAY_MODE)
		{
			find_data();
			Sleep(display_state.interval);
		}
		else if (display_state.mode == NO_MODE)
		{
			Sleep(1000);
		}
	}
}