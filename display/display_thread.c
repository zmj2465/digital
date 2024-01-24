#include "display_thread.h"
#include "display_send_thread.h"
#include "angle.h"
#include "physical_simulation.h"
#include "display_store_thread.h"
#include "comcal_dll.h"

#define FRESH_TIME 20

int ffflag = 0;
double te;
void* display_thread(void* arg)
{
	pthread_detach(pthread_self());
	int sel=0;
	int antenna_id;
	float azimuth;
	float elevation;
	while (1)
	{
		if (display_state.mode == SIM_MODE)
		{
			send_display_msg();

			double distance = caculate_distance(overall_fddi_info[0].pos, overall_fddi_info[1].pos)/5;
			//printf("%f\n", te=fmax(te, overall_fddi_info[0].pos.y));
			//printf("%f\n", overall_fddi_info[0].pos.y);
			if (ffflag == 0)
			{
				if (distance > 0.01)
				{
					//printf("distance=%f\n", overall_fddi_info[0].pos.y);
					Sleep(FRESH_TIME);
					generate_key_event(KEY_SEPARATE,0,0);
					ffflag = 1;
					display_state.flag = 1;
					//Sleep(FRESH_TIME);
					//generate_key_event(5, 1, 1);
				}
			}
			else if (ffflag == 1)
			{
				if (distance > 0.5) //Ω®¡¥
				{
					display_state.flag = 0;
					Sleep(FRESH_TIME);
					if (MY_INDEX == 0)
					{
						generate_key_event(5, 1, 1);
						Sleep(FRESH_TIME);
						//generate_key_event(5, 2, 1);
						//Sleep(FRESH_TIME);
						//generate_key_event(5, 3, 1);
						//Sleep(FRESH_TIME);
						//generate_key_event(5, 4, 1);
						//Sleep(FRESH_TIME);
					}
					else
						generate_key_event(5, 0, 0);
					ffflag = 2;
					//while (1);
				}
			}
			else if (ffflag == 2)
			{
				if (distance > 1)//∂œ¡¥
				{
					display_state.flag = 1;
					Sleep(FRESH_TIME);
					if (MY_INDEX == 0)
						generate_key_event(6, 1, 1);
					else
						generate_key_event(6, 0, 0);
					ffflag = 3;
				}
			}
			else if (ffflag == 3)
			{
				if (distance > 1.3)//‘ŸΩ®¡¥
				{
					display_state.flag = 0;
					Sleep(FRESH_TIME);
					if (MY_INDEX == 0)
					{
						generate_key_event(7, 1, 1);
					}
					else
						generate_key_event(7, 0, 0);
					ffflag = 4;
				}
			}
			Sleep(FRESH_TIME);
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


static int temp;
