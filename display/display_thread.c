#include "display_thread.h"
#include "display_send_thread.h"
#include "angle.h"
#include "physical_simulation.h"

int flag = 0;

void* display_thread(void* arg)
{
	pthread_detach(pthread_self());

	while (1)
	{
		if (display_state.mode == SIM_MODE)
		{
			send_display_msg();
			double distance = caculate_distance(fddi_info.pos, overall_fddi_info[1].pos);
			printf("distance=%f\n", distance);
			if (flag == 0)
			{
				if (distance > 1)
				{
					generate_key_event(4);
					flag = 1;
				}
			}
			Sleep(5);
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