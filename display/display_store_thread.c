#include "display_store_thread.h"
#include "display_send_thread.h"
#include "display_thread.h"
#include "physical_simulation.h"
#include "math.h"
#include "stdio.h"
#include "angle.h"
#include "comcal_dll.h"


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
            //data_store();
            //if (display_state.flag == 1)
            //{
            //    display_data.antenna_params[0].tx_rx_status = 0;
            //    display_data.antenna_params[1].tx_rx_status = 0;
            //    display_data.antenna_params[2].tx_rx_status = 0;
            //    display_data.antenna_params[3].tx_rx_status = 0;
            //    display_data.antenna_params[4].tx_rx_status = 0;
            //    display_data.antenna_params[5].tx_rx_status = 0;
            //    display_data.antenna_params[0].beam_width = 0;
            //    display_data.antenna_params[1].beam_width = 0;
            //    display_data.antenna_params[2].beam_width = 0;
            //    display_data.antenna_params[3].beam_width = 0;
            //    display_data.antenna_params[4].beam_width = 0;
            //    display_data.antenna_params[5].beam_width = 0;
            //    

            //    display_data.antenna_params[ttround].tx_rx_status = 2;
            //    display_data.antenna_params[ttround].beam_width = 30;
            //    ttround = (ttround + 1) % 6;
            //}
          

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
    send_to_display(&msg, msg.len);
}

static fddi_info_t temp_info[5];

void create_msg(show_t* msg)
{
    float yaw, pitch;
    set_zero(msg);
    int i;
    int j;
    int select_id = 0;

    int antenna_id=0;
    float azimuth;
    float elevation;
    msg->type = DISPLAY_INFO;

    msg->len = MAX_SEND_LEN;

    for (i = 0; i < 5; i++)
    {
        temp_info[i] = overall_fddi_info[i];
    }
    temp_info[0].pos.x = 0;
    temp_info[0].pos.y = 0;
    temp_info[0].pos.z = 0;
    temp_info[0].q.q0 = 0.1;
    temp_info[0].q.q1 = 0.1;
    temp_info[0].q.q2 = 0.1;
    temp_info[0].q.q3 = 0.1;

    temp_info[1].pos.x = 0;
    temp_info[1].pos.y = 50;
    temp_info[1].pos.z = 0;
    temp_info[1].q.q0 = 0.1;
    temp_info[1].q.q1 = 0.1;
    temp_info[1].q.q2 = 0.1;
    temp_info[1].q.q3 = 0.1;


    msg->display_info.serial_number = 0;
    msg->display_info.system_time.tv_sec = 0;// ����ϵͳʱ��Ϊ 2021-06-21 12:00:00
    msg->display_info.system_time.tv_nsec = 0;
    //λ����Ϣ
    msg->display_info.pos_x = temp_info[0].pos.x ;
    msg->display_info.pos_y = temp_info[0].pos.y ;
    msg->display_info.pos_z = temp_info[0].pos.z ;
    msg->display_info.vel_x = 1.0;
    msg->display_info.vel_y = 2.0;
    msg->display_info.vel_z = 3.0;
    msg->display_info.ang_vel_x = 0.1;
    msg->display_info.ang_vel_y = 0.2;
    msg->display_info.ang_vel_z = 0.3;

    msg->display_info.time_element_number = 0;
    msg->display_info.time_frame_number = 0;
    msg->display_info.micro_time_slot_number = 0;
    msg->display_info.node_role = MY_ROLE;
    msg->display_info.node_id = MY_INDEX - MY_ROLE;
    msg->display_info.link_status = 1;

    //�������ڵ���Ϣ
    if (MY_INDEX == 0) {
        double distance = caculate_distance(temp_info[0].pos, temp_info[1].pos);

        msg->display_info.z1_m_distance[1] = 1;
        //printf("distance %f %f\n", msg->display_info.z1_m_distance[1],distance);

        calculateYawAndPitch(temp_info[MY_INDEX].pos,
            temp_info[MY_INDEX].q,
            temp_info[1].pos,
            &yaw,
            &pitch);

        msg->display_info.z1_m_azimuth[1] = yaw;
        msg->display_info.z1_m_elevation[1] = pitch;

    }
    else if (MY_INDEX == 1)
    {
        double distance = caculate_distance(temp_info[1].pos, temp_info[0].pos);

        msg->display_info.z1_m_distance[0] = distance / 50;
        //printf("distance %f\n", msg->display_info.z1_m_distance[0]);

        calculateYawAndPitch(temp_info[MY_INDEX].pos,
            temp_info[MY_INDEX].q,
            temp_info[0].pos,
            &yaw,
            &pitch);

        msg->display_info.z1_m_azimuth[0] = yaw;
        msg->display_info.z1_m_elevation[0] = pitch;

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

    for (i = 0; i < 2; i++)
    {
        quaternionToEulerAngles(
            temp_info[i].q,
            &msg->display_info.roll[i],
            &msg->display_info.pitch[i],
            &msg->display_info.yaw[i]
        );
    }

    calculate_ante_angle_coord_m(
        temp_info[0].pos.x,
        temp_info[0].pos.y,
        temp_info[0].pos.z,
        temp_info[0].q.q0,
        temp_info[0].q.q1,
        temp_info[0].q.q2,
        temp_info[0].q.q3,
        0,
        temp_info[1].pos.x,
        temp_info[1].pos.y,
        temp_info[1].pos.z,
        &antenna_id,
        &azimuth,
        &elevation
    );
    antenna_id = antenna_id - 1;
    if (antenna_id >= 0 && antenna_id < 6);
    {
        msg->display_info.antenna_params[antenna_id].tx_rx_status = 1;
        msg->display_info.antenna_params[antenna_id].beam_width = 10;
        msg->display_info.antenna_params[antenna_id].elevation = elevation;
        msg->display_info.antenna_params[antenna_id].azimuth = azimuth;
    }
    data_show(msg);
}

void create_table(show_t* msg)
{
    int i, j, k;
    int ret = 0;
    //ѡ��һ���ڵ�
    for (i = 0; i < 5; i++)
    {
        //�ýڵ�δ����
        if (online_state[i] == 0) continue;
        //��������4���ڵ�
        for (j = 0; j < 5; j++)
        {
            if (j == i) continue;
            //��ýڵ�δ����
            if (online_state[j] == 0) continue;
            //i�ڵ�ѡ��ret�����߶���j
            ret = select_antennaB(i, temp_info, temp_info[j].pos);
            //printf("i=%d selant=%d\n", i, ret);
            if (ret < 0)
            {
                continue;
            }
            printf("i=%d selant=%d\n", i, ret);
            msg->display_info.link_target[i][ret] |= 1 << j;
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
    msg->display_info.link_target[0][1] |= 1 << 1;
    msg->display_info.link_target[1][5] |= 1 << 0;

    //msg->display_info.link_target[0][5] |= 1 << 2;
    //msg->display_info.link_target[2][1] |= 1 << 0;

    //msg->display_info.link_target[0][3] |= 1 << 3;
    //msg->display_info.link_target[3][1] |= 1 << 0;

    //msg->display_info.link_target[0][4] |= 1 << 4;
    //msg->display_info.link_target[4][1] |= 1 << 0;

}

void quaternionToEulerAngles(const Quaternion q, float* roll, float* pitch, float* yaw) {

    double q0 = q.q0;
    double q1 = q.q1;
    double q2 = q.q2;
    double q3 = q.q3;

    double q11 = q0 * q0;
    double q12 = q0 * q1;
    double q13 = q0 * q2;
    double q14 = q0 * q3;
    double q22 = q1 * q1;
    double q23 = q1 * q2;
    double q24 = q1 * q3;
    double q33 = q2 * q2;
    double q34 = q2 * q3;
    double q44 = q3 * q3;

    *roll = atan2(-2 * (q34 - q12), q11 - q22 + q33 - q44);
    *pitch = asin(2 * (q23 + q14));
    *yaw = atan2(-2 * (q24 - q13), q11 + q22 - q33 - q44);
    *roll = *roll * (180.0 / PI);
    *pitch = *pitch * (180.0 / PI);
    *yaw = *yaw * (180.0 / PI);
}


void data_show(show_t* msg)
{
    int i = 0;
    printf("0:x=%f,y=%f,z=%f,q0=%f,q1=%f,q2=%f,q3=%f\n",
        msg->display_info.pos_x,
        msg->display_info.pos_y,
        msg->display_info.pos_z,
        temp_info[0].q.q0,
        temp_info[0].q.q1,
        temp_info[0].q.q2,
        temp_info[0].q.q3
    );
    printf("1:x=%f,y=%f,z=%f,q0=%f,q1=%f,q2=%f,q3=%f\n",
        temp_info[1].pos.x,
        temp_info[1].pos.y,
        temp_info[1].pos.z,
        temp_info[1].q.q0,
        temp_info[1].q.q1,
        temp_info[1].q.q2,
        temp_info[1].q.q3
    );
    for (i = 0; i < 6; i++)
    {
        printf("state=%d,angle=%d,azimuth=%f,elevation=%f\n",
            msg->display_info.antenna_params[i].tx_rx_status,
            msg->display_info.antenna_params[i].beam_width,
            msg->display_info.antenna_params[i].azimuth,
            msg->display_info.antenna_params[i].elevation
        );
    }
    printf("0:pitch=%.12f,yaw=%.12f,roll=%.12f\n",
        msg->display_info.pitch[0],
        msg->display_info.yaw[0],
        msg->display_info.roll[0]
    );
    printf("1:pitch=%.12f,yaw=%.12f,roll=%.12f\n",
        msg->display_info.pitch[1],
        msg->display_info.yaw[1],
        msg->display_info.roll[1]
    );

    printf("azimuth=%f,elevation=%f\n", msg->display_info.z1_m_azimuth[1], msg->display_info.z1_m_elevation[1]);


}