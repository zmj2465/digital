#include "display_store_thread.h"
#include "display_send_thread.h"
#include "display_thread.h"
#include "physical_simulation.h"
#include "stdio.h"
#include "angle.h"


static int ttround = 0;

void* display_store_thread(void* arg)
{
    int i = 0;
    display_data.antenna_params[0].beam_width = 0;
    display_data.antenna_params[1].beam_width = 0;
    display_data.antenna_params[2].beam_width = 0;
    display_data.antenna_params[3].beam_width = 0;
    display_data.antenna_params[4].beam_width = 0;
    display_data.antenna_params[5].beam_width = 0;

    display_data.antenna_params[0].tx_rx_status = 0;
    display_data.antenna_params[1].tx_rx_status = 0;
    display_data.antenna_params[2].tx_rx_status = 0;
    display_data.antenna_params[3].tx_rx_status = 0;
    display_data.antenna_params[4].tx_rx_status = 0;
    display_data.antenna_params[5].tx_rx_status = 0;

    display_data.antenna_params[0].elevation = 90;
    display_data.antenna_params[1].elevation = 90;
    display_data.antenna_params[2].elevation = 90;
    display_data.antenna_params[3].elevation = 90;
    display_data.antenna_params[4].elevation = 90;
    display_data.antenna_params[5].elevation = 90;
    //display_state.mode = SIM_MODE;
	while (1)
	{
        //if (info.data_store_flag == 1)
        //{
        //    info.data_store_flag = 0;
        //    data_store();
        //}
        if (display_state.mode == SIM_MODE)
        {
            data_store();
            display_data.antenna_params[0].tx_rx_status = 0;
            display_data.antenna_params[1].tx_rx_status = 0;
            display_data.antenna_params[2].tx_rx_status = 0;
            display_data.antenna_params[3].tx_rx_status = 0;
            display_data.antenna_params[4].tx_rx_status = 0;
            display_data.antenna_params[5].tx_rx_status = 0;
            display_data.antenna_params[0].beam_width = 0;
            display_data.antenna_params[1].beam_width = 0;
            display_data.antenna_params[2].beam_width = 0;
            display_data.antenna_params[3].beam_width = 0;
            display_data.antenna_params[4].beam_width = 0;
            display_data.antenna_params[5].beam_width = 0;

            display_data.antenna_params[ttround].tx_rx_status = 2;
            display_data.antenna_params[ttround].beam_width = 30;
            ttround = (ttround + 1) % 6;

            //display_data.antenna_params[0].tx_rx_status = 0;
            //display_data.antenna_params[1].tx_rx_status = 1;
            //display_data.antenna_params[2].tx_rx_status = 2;
            //display_data.antenna_params[3].tx_rx_status = 0;
            //display_data.antenna_params[4].tx_rx_status = 1;
            //display_data.antenna_params[5].tx_rx_status = 2;
            Sleep(4);
        }
	}
}

float p = 0, pp = 0;

void data_store()
{
    show_t msg;
    create_msg(&msg);
    todata(&msg, msg.len);
}

void send_display_msg()
{
    show_t msg;
    create_msg(&msg);
    p++;
    pp++;
    send_to_display(&msg, msg.len);
}





void create_msg(show_t* msg)
{
    float yaw, pitch;
    set_zero(msg);
    int i;
    int j;
    msg->type = DISPLAY_INFO;
    //msg->len = 4 + sizeof(display_t);
    msg->len = MAX_SEND_LEN;

    msg->display_info.serial_number = 0;
    msg->display_info.system_time.tv_sec = 0;// 假设系统时间为 2021-06-21 12:00:00
    msg->display_info.system_time.tv_nsec = 0;
    //位置信息
    msg->display_info.pos_x = overall_fddi_info[0].pos.x / 100;
    msg->display_info.pos_y = overall_fddi_info[0].pos.y;
    msg->display_info.pos_z = overall_fddi_info[0].pos.z / 100;

    msg->display_info.vel_x = 1.0;
    msg->display_info.vel_y = 2.0;
    msg->display_info.vel_z = 3.0;
    msg->display_info.ang_vel_x = 0.1;
    msg->display_info.ang_vel_y = 0.2;
    msg->display_info.ang_vel_z = 0.3;
    msg->display_info.quat_q0 = 0.707;
    msg->display_info.quat_q1 = 0.0;
    msg->display_info.quat_q2 = 0.0;
    msg->display_info.quat_q3 = 0.707;
    msg->display_info.time_element_number = 0;
    msg->display_info.time_frame_number = 0;
    msg->display_info.micro_time_slot_number = 0;
    msg->display_info.node_role = MY_ROLE;
    msg->display_info.node_id = MY_INDEX - MY_ROLE;
    msg->display_info.link_status = 1;


    //与其他节点信息
    if (MY_INDEX == 0) {
        double distance = caculate_distance(fddi_info.pos, overall_fddi_info[1].pos);


        msg->display_info.z1_m_distance[1] = distance / 50;
        //printf("distance %f %f\n", msg->display_info.z1_m_distance[1],distance);

        calculateYawAndPitch(overall_fddi_info[MY_INDEX].pos,
            overall_fddi_info[MY_INDEX].q,
            overall_fddi_info[1].pos,
            &yaw,
            &pitch);

        msg->display_info.z1_m_azimuth[1] = yaw;
        msg->display_info.z1_m_elevation[1] = pitch;

        /*msg->display_info.z1_m_azimuth[1] = fmin(p, 45);
        msg->display_info.z1_m_elevation[1] = fmin(pp, 45);*/

    }
    else if (MY_INDEX == 1)
    {
        double distance = caculate_distance(fddi_info.pos, overall_fddi_info[0].pos);


        msg->display_info.z1_m_distance[0] = distance / 50;
        //printf("distance %f\n", msg->display_info.z1_m_distance[0]);

        calculateYawAndPitch(overall_fddi_info[MY_INDEX].pos,
            overall_fddi_info[MY_INDEX].q,
            overall_fddi_info[0].pos,
            &yaw,
            &pitch);

        msg->display_info.z1_m_azimuth[0] = yaw;
        msg->display_info.z1_m_elevation[0] = pitch;

        //msg->display_info.z1_m_azimuth[0] = fmin(p, 45);
        //msg->display_info.z1_m_elevation[0] = 180 - fmin(pp, 45);

    }

    msg->display_info.comm_status_mode = 2;
    msg->display_info.z_proc_flight_control_data_rx_tx_count = 0;
    msg->display_info.z_proc_flight_control_data_rx_tx_timestamp = 0;


    for (i = 0; i < 4; i++)
    {
        msg->display_info.z_m_send_recv_count[i] = 1;
    }
    msg->display_info.operation_status = 0;
    msg->display_info.channel_coding_decoding_frame_count = 0;
    msg->display_info.modulation_demodulation_frame_count = 0;
    msg->display_info.instruction_parsing_frame_count = 0;
    msg->display_info.m_node_time_freq_sync_status = 0;
    msg->display_info.m_node_downlink_link_status = 0;
    msg->display_info.m_node_beam_azimuth_direction = 0;
    msg->display_info.m_node_beam_elevation_direction = 90;


    msg->display_info.frequency_synthesizer_status = 0;
    msg->display_info.terminal_working_status_representation = 0;
    int temp = 0;
    msg->display_info.terminal_working_status_representation = temp;

    msg->display_info.antenna_params[0] = display_data.antenna_params[0];

    for (j = 0; j < 6; j++)
    {
        msg->display_info.antenna_params[j] = display_data.antenna_params[j];
        //printf("%d", msg->display_info.antenna_params[j].beam_width);
        //msg->display_info.antenna_params[j].tx_rx_status = 1;
        //msg->display_info.antenna_params[j].beam_width = 30;
        //msg->display_info.antenna_params[j].azimuth = 0;
        //msg->display_info.antenna_params[j].elevation = 90;
        //msg->display_info.antenna_params[j].eirp = 0;
        //msg->display_info.antenna_params[j].gt = 0;
    }

    for (j = 0; j < 4; j++)
    {
        msg->display_info.channel_params[j].node = 1;
        msg->display_info.channel_params[j].packet_loss_rate = 0;
        msg->display_info.channel_params[j].error_rate = 0;
        msg->display_info.channel_params[j].snr = 20.5;
        msg->display_info.channel_params[j].received_signal_power = -70.2;
        msg->display_info.channel_params[j].spreading_gain = 12.3;
        msg->display_info.channel_params[j].equivalent_spreading_factor = 4.5;
        msg->display_info.channel_params[j].noise_level = -90.1;
        msg->display_info.channel_params[j].distance = 500.0;
        msg->display_info.channel_params[j].path_loss = 60.8;
        msg->display_info.channel_params[j].transmission_delay = 0.003;
        msg->display_info.channel_params[j].doppler_shift = 100.5;
        msg->display_info.channel_params[j].radial_velocity = 50.2;
        msg->display_info.channel_params[j].beam_angle = 30.0;
        msg->display_info.channel_params[j].antenna_gain = 18.7;
        msg->display_info.channel_params[j].equivalent_isotropic_radiated_power = 30.5;
        msg->display_info.channel_params[j].transmitter_output_power = 25.8;
        if (j == 0)
            msg->display_info.channel_params[j].state = 1;
        if (j == 1)
            msg->display_info.channel_params[j].state = 0;
        if (j == 2)
            msg->display_info.channel_params[j].state = 1;
        if (j == 3)
            msg->display_info.channel_params[j].state = 0;
    }

    create_table(msg);
}