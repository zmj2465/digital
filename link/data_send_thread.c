#include "data_send_thread.h"
#include "file_manage.h"


m_gui_send_t m_gui_send;
m_gui_recv_t m_gui_recv;
m_tom_send_t m_tom_send;
m_tom_recv_t m_tom_recv;

z_gui_send_t z_gui_send;
z_gui_recv_t z_gui_recv;
z_tom_send_t z_tom_send;
z_tom_recv_t z_tom_recv;
common_data_t common_data[MZ_DATA];


/*
功能：发送数据线程
参数：无
返回值：无
*/
void* data_send_thread(void* arg)
{
    pthread_detach(pthread_self());
    set_thread_priority();

    while (1)
    {
        if (info.time_schedule_flag == 1)
        {
            info.time_schedule_flag = 0;
            data_send_proc();
        }
    }
}

/*
功能：数据发送的处理
参数：无
返回值：无
*/
int data_send_proc(void)
{
    psy_msg_t pmsg;
    msg_t msg;
    int i;
    int index;
    memset(&msg, 0, sizeof(msg_t));
    memset(&pmsg, 0, sizeof(psy_msg_t));
    /*主机M*/
    if (MY_INDEX == 0)
    {
        switch (fsm_status)
        {
        /*建链状态，按时隙进行扫描询问和数据发送*/
        case FSM_WSN:
            if(info.current_slot == 0 || info.current_slot == 5)//信令时隙
            {
                for (i = 1; i < FD_NUM; i++)
                {
                    if (inquire_index(i) == -1)//对应终端Z不在网
                    {
                        msg.data[0] = SCAN_REQ;
                        msg.len = 1;
                        /*发送扫描请求帧*/
                        generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], SCAN, &msg);

                        psy_send_(&pmsg,&msg);
                        //psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);

                        send(FD[i].fd, &pmsg, sizeof(psy_msg_t), 0);
                        plog("M send Z%d scan require, current slot = %d.%d, seq = %d\n", i, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                        ///*打开扫描响应定时器*/
                        //info.timerId_M[i] = timeSetEvent(TIMER_DELAY, 0, TimerCallback, SCAN_RES_TIMER, TIME_ONESHOT);
                    }
                }
            }
            else if(info.current_slot == 61)
            {
                /*测距时隙*/
                
            }
            else if(18 <= info.current_slot && info.current_slot <= 28)
            {
                /*M1_SEND_M2，M1_SNED_Z5*/
              
            }
            else//数据时隙
            {
                for (i = 1; i < FD_NUM; i++)
                {                   
                    index = schedule_inquire_index(i, info.current_slot);
                    if (index != -1)
                    {
                        if (info.scan_flag_M[index] == 1)//标志位置1，发送扫描确认帧
                        {
                            msg.data[0] = SCAN_CON;
                            msg.data[1] = info.current_time_frame + 1;
                            msg.len = 1;
                            generate_packet(info.device_info.node_id[index], info.device_info.node_id[MY_INDEX], SCAN, &msg);
                            //psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                            psy_send_(&pmsg, &msg);
                            send(FD[index].fd, &pmsg, sizeof(psy_msg_t), 0);
                            info.scan_flag_M[index] = 0;
                            info.time_frame_flag_m[index] = info.current_time_frame + 1;
                            plog("M send Z%d scan confirm, current slot = %d.%d, seq = %d\n", index, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                            if (info.device_info.node_num == FD_NUM)
                            {
                                fsm_do(EVENT_WSN_SUCC);
                            }
                            return 0;
                        }
                        else//标志位置0，发送数据帧
                        {
                            if (info.current_time_frame >= info.time_frame_flag_m[index])
                            {
                                msg.data[0] = 5;
                                msg.len = 1;
                                generate_packet(info.device_info.node_id[index], info.device_info.node_id[MY_INDEX], LONG_FRAME, &msg);
                                //psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                                psy_send_(&pmsg, &msg);
                                send(FD[index].fd, &pmsg, sizeof(psy_msg_t), 0);
                                plog("M send Z%d data, current slot = %d.%d, seq = %d\n", index, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                            } 
                            return 0;
                        }
                    }
                }
                for (i = 1; i < FD_NUM; i++)
                {
                    if (inquire_index(i) == -1)//对应终端Z不在网，发送扫描请求帧
                    {
                        msg.data[0] = SCAN_REQ;
                        msg.len = 1;
                        generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], SCAN, &msg);
                        //psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                        psy_send_(&pmsg, &msg);
                        send(FD[i].fd, &pmsg, sizeof(psy_msg_t), 0);
                        plog("M send Z%d scan require, current slot = %d.%d, seq = %d\n", i, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                        /*打开扫描响应定时器*/
                        //info.timerId_M[i] = timeSetEvent(TIMER_DELAY, 0, TimerCallback, SCAN_RES_TIMER, TIME_ONESHOT);
                    }
                }                
            }
            break;
        /*建链完成状态*/
        case FSM_ON:
            if (info.current_slot == 0 || info.current_slot == 5)//信令时隙
            {
                for (i = 1; i < FD_NUM; i++)
                {
                    index = beacon_m_inquire_index(i, info.current_slot, info.current_time_frame);
                    if (index != -1)
                    {
                        msg.data[0] = 6;
                        msg.len = 1;
                        generate_packet(info.device_info.node_id[index], info.device_info.node_id[MY_INDEX], BEACON, &msg);
                        //psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                        psy_send_(&pmsg, &msg);
                        send(FD[index].fd, &pmsg, sizeof(psy_msg_t), 0);
                        plog("M send Z%d beacon, current slot = %d.%d, seq = %d\n", index, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                        return 0;
                    }                    
                }
            }
            else if(info.current_slot == 61)//测距时隙
            {
                for (i = 1; i < FD_NUM; i++)
                {
                    if (info.distance_flag_M[i] == 1)
                    {
                        msg.data[0] = DISTANCE_M;
                        msg.len = 1;
                        generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], DISTANCE, &msg);
                        //psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                        psy_send_(&pmsg, &msg);
                        send(FD[i].fd, &pmsg, sizeof(psy_msg_t), 0);
                        plog("M send Z%d distance frame, current slot = %d.%d, seq = %d\n", i, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                        info.distance_flag_M[i] = 0;
                    }
                }

            }
            else if(18 <= info.current_slot && info.current_slot <= 28)
            {
                /*M1_SEND_M2，M1_SNED_Z5*/
            }
            else//数据时隙Z1-Z4
            {
                for (i = 1; i < FD_NUM; i++)
                {
                    index = schedule_inquire_index(i, info.current_slot);
                    if (index != -1)
                    {
                        if (info.current_time_frame >= info.time_frame_flag_m[index])
                        {
                            msg.data[0] = 5;
                            msg.len = 1;
                            generate_packet(info.device_info.node_id[index], info.device_info.node_id[MY_INDEX], LONG_FRAME, &msg);
                            //psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                            psy_send_(&pmsg, &msg);
                            send(FD[index].fd, &pmsg, sizeof(psy_msg_t), 0);
                            plog("M send Z%d data, current slot = %d.%d, seq = %d\n", index, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                        }
                        return 0;    
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    /*从机Z*/
    else
    {
        switch (fsm_status)
        {
        /*建链状态*/
        case FSM_WAN:
            if (info.current_slot != 59)//扫描时隙
            {
                /*发送扫描响应帧*/
                if (info.scan_flag_Z == 1)
                {
                    msg.data[0] = SCAN_RES;
                    msg.len = 1;
                    generate_packet(info.device_info.node_id[0], info.device_info.node_id[MY_INDEX], SCAN, &msg);
                    //psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                    psy_send_(&pmsg, &msg);
                    send(FD[0].fd, &pmsg, sizeof(psy_msg_t), 0);
                    plog("Z%d send scan response, current slot = %d.%d, seq = %d\n", MY_INDEX, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);                
#ifdef _WIN32
                    /*打开扫描确认定时器*/
                    info.timerId = timeSetEvent(TIMER_DELAY, 0, TimerCallback, SCAN_CON_TIMER, TIME_ONESHOT);
#endif
                    info.scan_flag_Z = 0;
                }
            }
            break;
        /*建链完成状态*/
        case FSM_ON:
            if (info.current_slot == 30 || info.current_slot == 35)//信令时隙
            {
                index = beacon_z_inquire_index(MY_INDEX, info.current_slot, info.current_time_frame);
                if (index != -1)
                {
                    msg.data[0] = 6;
                    msg.len = 1;
                    generate_packet(info.device_info.node_id[0], info.device_info.node_id[MY_INDEX], BEACON, &msg);
                    //psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                    psy_send_(&pmsg, &msg);
                    send(FD[0].fd, &pmsg, sizeof(psy_msg_t), 0);
                    plog("Z%d send M beacon, current slot = %d.%d, seq = %d\n", MY_INDEX, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                }
            }
            else if (info.current_slot == 59)//测距时隙
            {
                if ((info.current_time_frame % 4) + 1 == MY_INDEX)//每4个时帧给M发测距帧
                {
                    msg.data[0] = DISTANCE_Z;
                    msg.len = 1;
                    generate_packet(info.device_info.node_id[0], info.device_info.node_id[MY_INDEX], DISTANCE, &msg);
                    //psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                    psy_send_(&pmsg, &msg);
                    send(FD[0].fd, &pmsg, sizeof(psy_msg_t), 0);
                    plog("Z%d send distance frame, current slot = %d.%d, seq = %d\n", MY_INDEX, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                }
            }
            else//数据时隙
            {
                index = inquire_node_index(MY_INDEX, info.current_slot);
                if (index != -1)
                {
                    if (info.current_time_frame >= info.time_frame_flag_z)
                    {
                        msg.data[0] = 5;
                        msg.len = 1;
                        generate_packet(info.device_info.node_id[0], info.device_info.node_id[MY_INDEX], LONG_FRAME, &msg);
                        //psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                        psy_send_(&pmsg, &msg);
                        send(FD[0].fd, &pmsg, sizeof(psy_msg_t), 0);
                        plog("Z%d send M data, current slot = %d.%d, seq = %d\n", MY_INDEX, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    return 0;
}

/*
功能：组包添加帧头生成数据帧
参数：目的地址，源地址，当前系统时间，帧类型，位置信息等
返回值：无
*/
void generate_packet(uint8_t dst, uint8_t src, uint8_t type, msg_t* msg)
{
    int index;
    msg->head.dst = dst;
    msg->head.src = src;
    msg->head.type = type;
    
    if (type == START_GUN && MY_INDEX == 0)
    {
        //msg->head.send_time = info.str.base_time;
        msg->head.send_t = info.str.base_t;
    }
    else
    {
        //clock_gettime(CLOCK_REALTIME, &msg->head.send_time);
        msg->head.send_t = my_get_time();
    }

    if (MY_INDEX == 0)//M
    {
        if (type == SCAN && msg->data[0] == SCAN_REQ)
        {
            msg->head.antenna_id = info.current_antenna;
        }
        else
        {
            index = inquire_address(msg->head.dst);
            msg->head.antenna_id = info.antenna_M[index];
        }


        if (type == BEACON)
        {
            msg->head.seq = info.seq_beacon_m;
            info.seq_beacon_m++;
        }
        else if (type == DISTANCE)
        {
            msg->head.seq = info.seq_distance_m;
            info.seq_distance_m++;
        }
        else
        {
            msg->head.seq = info.seq_m;
            info.seq_m++;
        }
    }
    else//Z
    {
        msg->head.antenna_id = info.antenna_Z;

        if (type == BEACON)
        {
            msg->head.seq = info.seq_beacon_z;
            info.seq_beacon_z++;
        }
        else if (type == DISTANCE)
        {
            msg->head.seq = info.seq_distance_z;
            info.seq_distance_z++;
        }
        else
        {
            msg->head.seq = info.seq_z;
            info.seq_z++;
        }
    }

    msg->len = msg->len + sizeof(head_t) + sizeof(int);//加上包头长度
    /*位置信息待补充*/
}


//m_gui_send_t m_gui_send;
//m_gui_recv_t m_gui_recv;
//m_tom_send_t m_tom_send;
//m_tom_recv_t m_tom_recv;
//
//z_gui_send_t z_gui_send;
//z_gui_recv_t z_gui_recv;
//z_tom_send_t z_tom_send;
//z_tom_recv_t z_tom_recv;


//void get_m_gui_send(char* buff,int index)
//{
//    pthread_mutex_lock(&m_gui_send.lock);
//    memcpy(buff, &m_gui_send.data[index * M_GUI_SEND_LEN], M_GUI_SEND_LEN);
//    pthread_mutex_unlock(&m_gui_send.lock);
//}
//
//void put_m_gui_send(char* buff, int index)
//{
//    pthread_mutex_lock(&m_gui_recv.lock);
//    memcpy(&m_gui_recv.data[index * M_GUI_RECV_LEN], buff, M_GUI_RECV_LEN);
//    pthread_mutex_lock(&m_gui_recv.lock);
//}
//
//
//void get_m_tom_send(char* buff, int index)
//{
//    pthread_mutex_lock(&m_tom_send.lock);
//    memcpy(buff, &m_tom_send.data[index * M_TOM_SEND_LEN], M_TOM_SEND_LEN);
//    pthread_mutex_lock(&m_tom_send.lock);
//}
//
//void put_m_tom_send(char* buff, int index)
//{
//    pthread_mutex_lock(&m_tom_recv.lock);
//    memcpy(&m_tom_recv.data[index * M_TOM_RECV_LEN], buff, M_TOM_RECV_LEN);
//    pthread_mutex_lock(&m_tom_recv.lock);
//}
//
//
//void get_z_gui_send(char* buff)
//{
//    pthread_mutex_lock(&z_gui_send.lock);
//    memcpy(buff, z_gui_send.data, Z_GUI_SEND_LEN);
//    pthread_mutex_lock(&z_gui_send.lock);
//}
//
//void put_z_gui_send(char* buff)
//{
//    pthread_mutex_lock(&z_gui_recv.lock);
//    memcpy(z_gui_recv.data, buff, Z_GUI_RECV_LEN);
//    pthread_mutex_lock(&z_gui_recv.lock);
//}
//
//
//void get_z_tom_send(char* buff)
//{
//    pthread_mutex_lock(&z_tom_send.lock);
//    memcpy(buff, z_tom_send.data, Z_TOM_SEND_LEN);
//    pthread_mutex_lock(&z_tom_send.lock);
//}
//
//void put_z_tom_send(char* buff)
//{
//    pthread_mutex_lock(&z_tom_recv.lock);
//    memcpy(z_tom_recv.data, buff, Z_TOM_RECV_LEN);
//    pthread_mutex_lock(&z_tom_recv.lock);
//}


void get(common_data_t* src,char* dst,int len,int index)
{
    pthread_mutex_lock(&src->lock);
    memcpy(dst, src->data + index * len, len);
    pthread_mutex_unlock(&src->lock);
}


void put(common_data_t* dst, char* src, int len, int index)
{
    pthread_mutex_lock(&dst->lock);
    memcpy(dst->data + index * len, src, len);
    pthread_mutex_unlock(&dst->lock);
}

