#include "display_thread.h"
#include "display_send_thread.h"
#include "angle.h"
#include "physical_simulation.h"
#include "display_store_thread.h"
#include "comcal_dll.h"

#define FRESH_TIME 20

int flag = 0;
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
			//printf("ok\n");
			double distance = caculate_distance(overall_fddi_info[0].pos, overall_fddi_info[1].pos)/5;
			//printf("%f\n", te=fmax(te, overall_fddi_info[0].pos.y));
			//printf("%f\n", overall_fddi_info[0].pos.y);
			if (flag == 0)
			{
				if (distance > 0.01)
				{
					//printf("distance=%f\n", overall_fddi_info[0].pos.y);
					Sleep(FRESH_TIME);
					generate_key_event(KEY_SEPARATE,0,0);
					flag = 1;
					display_state.flag = 1;
				}
			}
			else if (flag == 1)
			{
				if (distance > 1) //建链
				{
					display_state.flag = 0;
					Sleep(FRESH_TIME);
					if (MY_INDEX == 0)
					{
						//display_data.antenna_params[0].tx_rx_status = 0;
						//display_data.antenna_params[1].tx_rx_status = 0;
						//display_data.antenna_params[2].tx_rx_status = 0;
						//display_data.antenna_params[3].tx_rx_status = 0;
						//display_data.antenna_params[4].tx_rx_status = 0;
						//display_data.antenna_params[5].tx_rx_status = 0;
						//display_data.antenna_params[0].beam_width = 0;
						//display_data.antenna_params[1].beam_width = 0;
						//display_data.antenna_params[2].beam_width = 0;
						//display_data.antenna_params[3].beam_width = 0;
						//display_data.antenna_params[4].beam_width = 0;
						//display_data.antenna_params[5].beam_width = 0;
						//sel= select_antennaA(MY_INDEX, overall_fddi_info[1].pos);
						//display_data.antenna_params[sel].tx_rx_status = 1;
						//display_data.antenna_params[sel].beam_width = 60;
						//calculate_ante_angle_coord_m(
						//	overall_fddi_info[0].pos.x,
						//	overall_fddi_info[0].pos.y,
						//	overall_fddi_info[0].pos.z,
						//	overall_fddi_info[0].q.q0,
						//	overall_fddi_info[0].q.q1,
						//	overall_fddi_info[0].q.q2,
						//	overall_fddi_info[0].q.q3,
						//	0,
						//	overall_fddi_info[1].pos.x,
						//	overall_fddi_info[1].pos.y,
						//	overall_fddi_info[1].pos.z,
						//	&antenna_id,
						//	&azimuth,
						//	&elevation
						//);
						//display_data.antenna_params[antenna_id].tx_rx_status = 1;
						//display_data.antenna_params[antenna_id].beam_width = 10;
						//display_data.antenna_params[antenna_id].elevation = elevation;
						//display_data.antenna_params[antenna_id].azimuth = azimuth;
						//while (1);
						generate_key_event(5, 1, 1);
					}
					else
						generate_key_event(5, 0, 0);
					flag = 2;
				}
			}
			else if (flag == 2)
			{
				if (distance > 2)//断链
				{
					display_state.flag = 1;
					Sleep(FRESH_TIME);
					if (MY_INDEX == 0)
						generate_key_event(6, 1, 1);
					else
						generate_key_event(6, 0, 0);
					flag = 3;
				}
			}
			else if (flag == 3)
			{
				if (distance > 3)//再建链
				{
					display_state.flag = 0;
					Sleep(FRESH_TIME);
					if (MY_INDEX == 0)
					{
						//display_data.antenna_params[0].tx_rx_status = 0;
						//display_data.antenna_params[1].tx_rx_status = 0;
						//display_data.antenna_params[2].tx_rx_status = 0;
						//display_data.antenna_params[3].tx_rx_status = 0;
						//display_data.antenna_params[4].tx_rx_status = 0;
						//display_data.antenna_params[5].tx_rx_status = 0;
						//display_data.antenna_params[0].beam_width = 0;
						//display_data.antenna_params[1].beam_width = 0;
						//display_data.antenna_params[2].beam_width = 0;
						//display_data.antenna_params[3].beam_width = 0;
						//display_data.antenna_params[4].beam_width = 0;
						//display_data.antenna_params[5].beam_width = 0;
						//sel = select_antennaA(MY_INDEX, overall_fddi_info[1].pos);
						//display_data.antenna_params[sel].tx_rx_status = 1;
						//display_data.antenna_params[sel].beam_width = 60;
						/*calculate_ante_angle_coord_m(
							overall_fddi_info[0].pos.x,
							overall_fddi_info[0].pos.y,
							overall_fddi_info[0].pos.z,
							overall_fddi_info[0].q.q0,
							overall_fddi_info[0].q.q1,
							overall_fddi_info[0].q.q2,
							overall_fddi_info[0].q.q3,
							0,
							overall_fddi_info[1].pos.x,
							overall_fddi_info[1].pos.y,
							overall_fddi_info[1].pos.z,
							&antenna_id,
							&azimuth,
							&elevation
						);
						display_data.antenna_params[antenna_id].tx_rx_status = 1;
						display_data.antenna_params[antenna_id].beam_width = 10;
						display_data.antenna_params[antenna_id].elevation = elevation;
						display_data.antenna_params[antenna_id].azimuth = azimuth;*/
						generate_key_event(7, 1, 1);
					}
					else
						generate_key_event(7, 0, 0);
					flag = 4;
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
			//printf("%d\n", ret);
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
	//msg->display_info.link_target[0][1] |= 1 << 1;
	//msg->display_info.link_target[1][5] |= 1 << 0;
}