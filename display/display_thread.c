#include "display_thread.h"
#include "display_send_thread.h"
#include "angle.h"
#include "physical_simulation.h"
#include "display_store_thread.h"

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
					generate_key_event(4,0,0);
					flag = 1;
				}
			}
			else if (flag == 1)
			{
				if (distance > 1)
				{
					Sleep(2);
					if (MY_INDEX == 0)
						generate_key_event(5, 1, 1);
					else
						generate_key_event(5, 0, 0);
					flag = 2;
				}
			}
			else if (flag == 2)
			{
				if (distance > 2)
				{
					Sleep(2);
					if (MY_INDEX == 0)
						generate_key_event(6, 1, 1);
					else
						generate_key_event(6, 0, 0);
					flag = 3;
				}
			}
			else if (flag == 3)
			{
				if (distance > 3)
				{
					Sleep(2);
					if (MY_INDEX == 0)
						generate_key_event(7, 1, 1);
					else
						generate_key_event(7, 0, 0);
					flag = 4;
				}
			}
			Sleep(20);
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
void create_table(show_t* msg)
{
	int i, j, k;
	int ret = 0;
	//选择一个节点
	for (i = 0; i < 5; i++)
	{
		//该节点未在网
		if (online_state[i] == 0) continue;
		//遍历另外4个节点
		for (j = 0; j < 5; j++)
		{
			if (j == i) continue;
			//与该节点未建链
			if (online_state[j] == 0) continue;
			//i节点选择ret号天线对上j
			ret = select_antennaA(i, overall_fddi_info[j].pos);
			if (ret < 0)
			{
				continue;
			}
			printf("%d\n", ret);
			msg->display_info.link_target[i][temp] |= 1 << j;
 		}
	}
	//temp = (temp + 1) % 6;
	//printf("%%%%%%%%%%%%%%%%%%%%%%\n");
	//for (i = 0; i < 5; i++)
	//{
	//	for (j = 0; j < 6; j++)
	//	{
	//		printf("%d ", msg->display_info.link_target[i][j]);
	//	}
	//	printf("\n");
	//}
}