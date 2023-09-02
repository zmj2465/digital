#include "display_thread.h"
#include "display_send_thread.h"
#include "angle.h"
#include "physical_simulation.h"

int flag = 0;
double te;
void* display_thread(void* arg)
{
	pthread_detach(pthread_self());

	while (1)
	{
		if (display_state.mode == SIM_MODE)
		{
			send_display_msg();
			double distance = caculate_distance(overall_fddi_info[0].pos, overall_fddi_info[1].pos)/20;
			//printf("%f\n", te=fmax(te, overall_fddi_info[0].pos.y));
			//printf("%f\n", overall_fddi_info[0].pos.y);
			if (flag == 0)
			{
				if (distance > 0.01)
				{
					//printf("distance=%f\n", overall_fddi_info[0].pos.y);
					Sleep(1);
					generate_key_event(4);
					flag = 1;
				}
			}
			else if (flag == 1)
			{
				if (distance > 1)
				{
					Sleep(2);
					generate_key_event(5);
					Sleep(2);
					//flag = 2;
				}
			}
			Sleep(100);
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