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
int send_flag;

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
    int real_index;
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
                    real_index = inquire_socket_id(i);

                    if (inquire_index(real_index) == -1)//对应终端Z不在网
                    {
                        msg.data[0] = SCAN_REQ;
                        msg.len = 1;
                                                
                        /*发送扫描请求帧*/
                        generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], SCAN, &msg);                       
                        psy_send_(&pmsg,&msg);                                         
                        send(FD[i].fd, &pmsg, sizeof(psy_msg_t), 0);
                        plog("M send Z%d scan require, current slot = %d.%d, seq = %d\n", real_index, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
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

                            generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], SCAN, &msg);                           
                            psy_send_(&pmsg, &msg);
                            send(FD[i].fd, &pmsg, sizeof(psy_msg_t), 0);
                            info.scan_flag_M[index] = 0;
                            info.time_frame_flag_m[index] = info.current_time_frame + 1;
                            plog("M send Z%d scan confirm, current slot = %d.%d, seq = %d\n", index, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
							info.znode_connect_flag[i-1] = 1;//z1放入数组0的位置
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
                                /*测试帧*/
                                //msg.data[0] = 5;
                                                               
                                /*长帧发送*/                            
                                if (send_flag == M_GUI_SEND_LEN)//ZD数据
                                {
                                    msg.len = M_GUI_SEND_LEN;
                                    get(&common_data[M_GUI_SEND], msg.data, msg.len, index);
                                }
                                else if (send_flag == M_TOM_SEND_LEN)//TOM数据
                                {
                                    msg.len = M_TOM_SEND_LEN;
                                    get(&common_data[M_TOM_SEND], msg.data, msg.len, index);
                                }
                                printf("m send type=%02x\n", msg.data[0]);

                                
                                generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], LONG_FRAME, &msg);
								g_node_progrm[index].air_interface_data_tx_count++;//对端节点相应数据计数
                                psy_send_(&pmsg, &msg);
                                send(FD[i].fd, &pmsg, sizeof(psy_msg_t), 0); //#define FD info.simulated_link		
                                plog("M send Z%d data, current slot = %d.%d, seq = %d\n", index, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                            } 
                            return 0;
                        }
                    }
                }
                for (i = 1; i < FD_NUM; i++)
                {
                    real_index = inquire_socket_id(i);
                    if (inquire_index(real_index) == -1)//对应终端Z不在网，发送扫描请求帧
                    {
                        msg.data[0] = SCAN_REQ;
                        msg.len = 1;

                        
                        generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], SCAN, &msg);
                        
                        psy_send_(&pmsg, &msg);
                        send(FD[i].fd, &pmsg, sizeof(psy_msg_t), 0);
                        plog("M send Z%d scan require, current slot = %d.%d, seq = %d\n", real_index, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
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

                        generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], BEACON, &msg);
                        
                        psy_send_(&pmsg, &msg);
                        send(FD[i].fd, &pmsg, sizeof(psy_msg_t), 0);
                        plog("M send Z%d beacon, current slot = %d.%d, seq = %d\n", index, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                        return 0;
                    }                    
                }
            }
            else if(info.current_slot == 61)//测距时隙
            {
                for (i = 1; i < FD_NUM; i++)
                {
                    real_index = inquire_socket_id(i);
                    if (info.distance_flag_M[real_index] == 1)
                    {
                        msg.data[0] = DISTANCE_M;
                        msg.len = 1;

                        generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], DISTANCE, &msg);
                        
                        psy_send_(&pmsg, &msg);
                        send(FD[i].fd, &pmsg, sizeof(psy_msg_t), 0);
                        plog("M send Z%d distance frame, current slot = %d.%d, seq = %d\n", real_index, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                        info.distance_flag_M[real_index] = 0;
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

                            //长帧发送
                            if (send_flag == M_GUI_SEND_LEN)
                            {
                                msg.len = M_GUI_SEND_LEN;
                                get(&common_data[M_GUI_SEND], msg.data, msg.len, index);
                            }
                            else if (send_flag == M_TOM_SEND_LEN)
                            {
                                msg.len = M_TOM_SEND_LEN;
                                get(&common_data[M_TOM_SEND], msg.data, msg.len, index);
                            }

                            printf("m send type=%02x\n", msg.data[0]);

                            
                            generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], LONG_FRAME, &msg);
							g_node_progrm[index].air_interface_data_tx_count++; //M发往各个z的数据包计数
                            psy_send_(&pmsg, &msg);
                            send(FD[i].fd, &pmsg, sizeof(psy_msg_t), 0);
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
                    generate_packet(info.device_info.node_id[0], MY_ID, SCAN, &msg);
                    
                    psy_send_(&pmsg, &msg);
                    send(FD[0].fd, &pmsg, sizeof(psy_msg_t), 0);
                    plog("Z%d send scan response, current slot = %d.%d, seq = %d\n", MY_ID_INDEX, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);                
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
                index = beacon_z_inquire_index(MY_ID_INDEX, info.current_slot, info.current_time_frame);
                if (index != -1)
                {
                    msg.data[0] = 6;
                    msg.len = 1;
                    generate_packet(info.device_info.node_id[0], MY_ID, BEACON, &msg);
                    
                    psy_send_(&pmsg, &msg);
                    send(FD[0].fd, &pmsg, sizeof(psy_msg_t), 0);
                    plog("Z%d send M beacon, current slot = %d.%d, seq = %d\n", MY_ID_INDEX, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                }
            }
            else if (info.current_slot == 59)//测距时隙
            {
                if ((info.current_time_frame % 4) + 1 == MY_ID_INDEX)//每4个时帧给M发测距帧
                {
                    msg.data[0] = DISTANCE_Z;
                    msg.len = 1;
                    generate_packet(info.device_info.node_id[0], MY_ID, DISTANCE, &msg);
                    
                    psy_send_(&pmsg, &msg);
                    send(FD[0].fd, &pmsg, sizeof(psy_msg_t), 0);
                    plog("Z%d send distance frame, current slot = %d.%d, seq = %d\n", MY_ID_INDEX, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
                }
            }
            else//数据时隙
            {
                index = inquire_node_index(MY_ID_INDEX, info.current_slot);
                if (index != -1)
                {
                    if (info.current_time_frame >= info.time_frame_flag_z)
                    {

                        //长帧发送
                        if (send_flag == Z_GUI_SEND_LEN)
                        {
                            msg.len = Z_GUI_SEND_LEN;
                            get(&common_data[Z_GUI_SEND], msg.data, msg.len, 0);
                            send_flag = 0;
                        }
                        else if (send_flag == Z_TOM_SEND_LEN)
                        {
                            msg.len = Z_TOM_SEND_LEN;
                            get(&common_data[Z_TOM_SEND], msg.data, msg.len, 0);
                            send_flag = 0;
                        }

                        printf("z send type %02x\n", msg.data[0]);
                        generate_packet(info.device_info.node_id[0], MY_ID, LONG_FRAME, &msg);
						g_node_progrm[0].air_interface_data_tx_count++; //Zi发往M的数据包计数
                        psy_send_(&pmsg, &msg);
                        send(FD[0].fd, &pmsg, sizeof(psy_msg_t), 0);
                        plog("Z%d send M data, current slot = %d.%d, seq = %d\n", MY_ID_INDEX, info.current_time_frame, info.current_slot, pmsg.msg.head.seq);
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
    index = inquire_address(dst);

    /*添加发送时间*/
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

    /*添加序列号*/
    if (type == BEACON)
    {
        msg->head.seq = info.seq_beacon;
        info.seq_beacon++;
    }
    else if (type == DISTANCE)
    {
        msg->head.seq = info.seq_distance;
        info.seq_distance++;
    }
    else
    {
        msg->head.seq = info.seq_data;
        info.seq_data++;
    }

    ///*添加天线id*/
    //if (type == SCAN && msg->data[0] == SCAN_REQ)
    //{
    //    info.current_antenna = info.chain_antenna;
    //}
    //else
    //{
    //    info.current_antenna = select_antennaA(MY_INDEX, overall_fddi_info[index].pos);//根据索引判断当前的发送天线id       
    //}

    //msg->head.antenna_id = info.current_antenna;        //发送天线id
    msg->len = msg->len + sizeof(head_t) + sizeof(int);//加上包头长度

    ///*将发送天线状态信息推送到显控界面*/
    //set_antenna_parameter(info.current_antenna, index, 1);

    

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


void mget(common_data_t* src, char* dst, int len)
{
    pthread_mutex_lock(&src->lock);
    memcpy(dst, src->data + len / 4, len);
    pthread_mutex_unlock(&src->lock);
}


void mput(common_data_t* dst, char* src, int len)
{
    pthread_mutex_lock(&dst->lock);
    memcpy(dst->data + len / 4, src, len);
    pthread_mutex_unlock(&dst->lock);
}

void ppp(common_data_t* content,int len,int index)
{
    int i = 0;
    for (i = 0; i < len; i++)
    {
        printf("%02x ", content->data[i + index * len]);
    }
    printf("\n");
}

/*
作用：根据此时的socket编号，找到node_id[socket编号]里对应的真正的Zi
（比如输入id = 1，发现node_id[id] = 12H，说明此时socket编号为1的数组中存储的是z2）
（10H：m，11H：z1，12H：z2，13H：z3，14H：z4）
参数：socket编号
返回值：参数装订后Zi的编号
*/
int inquire_socket_id(uint8_t id)
{
    //id = id | 16;
    //int i;
    //for (i = 1; i < FD_NUM; i++)
    //{
    //    if (info.device_info.node_id[i] = id)
    //    {
    //        return i;
    //    }
    //}
    int i;
    
    i = info.device_info.node_id[id] - 0x10;
   
    return i;
}