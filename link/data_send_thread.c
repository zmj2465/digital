#include "data_send_thread.h"


/*
功能：发送数据线程
参数：无
返回值：无
*/
void* data_send_thread(void* arg)
{
    pthread_detach(pthread_self());

    while (1)
    {
        sem_wait(&info.send_semaphore);
      
        /*组包发送*/
        data_send_proc();

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
        case FSM_WSN://正在建链状态，按时隙进行扫描询问和数据发送
            if (info.current_slot == 0 || info.current_slot == 5)//信令时隙
            {
                for (i = 1; i < FD_NUM; i++)
                {
                    if (inquire_index(i) == -1)//对应终端Z不在网
                    {
                        msg.data[0] = SCAN_REQ;
                        msg.len = 1;
                        /*发送扫描询问帧*/
                        generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], SCAN, &msg);
                        psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                        send(FD[i].fd, &pmsg, MAX_DATA_LEN, 0);
                        printf("dst1 = %d\n", pmsg.msg.head.dst);
                        ///*打开扫描响应定时器*/
                        //info.timerId_M[i] = timeSetEvent(TIMER_DELAY, 0, TimerCallback, SCAN_RES_TIMER, TIME_ONESHOT);
                    }
                }
            }
            else if (info.current_slot == 61)
            {
                /*测距时隙*/
                
            }
            else if(18 <= info.current_slot && info.current_slot <= 28)
            {
                /*M1_SEND_M2，M1_SNED_Z5*/
              
            }
            else
            {
                for (i = 1; i < FD_NUM; i++)
                {
                    
                    index = schedule_inquire_index(i, info.current_slot);
                    if (index != -1)
                    {
                        if (info.scan_flag[index] == 1)
                        {
                            msg.data[0] = SCAN_CON;
                            msg.len = 1;
                            /*发送扫描回复帧*/
                            generate_packet(info.device_info.node_id[index], info.device_info.node_id[MY_INDEX], SCAN, &msg);
                            psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                            send(FD[index].fd, &pmsg, MAX_DATA_LEN, 0);
                            info.scan_flag[index] = 0;
                            printf("M send Z%d scan confirm successfully, current slot = %d\n", index, info.current_slot);
                            //if (info.device_info.node_num == FD_NUM)
                            //{
                                //fsm_do(EVENT_WSN_SUCC);
                            //}
                            return 0;
                        }
                        else
                        {
                            /*发送数据帧*/
                            //dequeue(&info.thread_queue[DATA_SEND_THREAD], msg.data, &msg.len);
                            msg.data[0] = 5;
                            msg.len = 1;
                            generate_packet(info.device_info.node_id[index], info.device_info.node_id[MY_INDEX], LONG_FRAME, &msg);
                            psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                            send(FD[index].fd, &pmsg, MAX_DATA_LEN, 0);
                            printf("M send Z%d successfully, current slot = %d\n", index, info.current_slot);
                            return 0;
                        }
                    }
                }
                for (i = 1; i < FD_NUM; i++)
                {
                    if (inquire_index(i) == -1)//对应终端Z不在网
                    {
                        msg.data[0] = SCAN_REQ;
                        msg.len = 1;
                        /*发送扫描询问帧*/
                        generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], SCAN, &msg);
                        psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                        send(FD[i].fd, &pmsg, MAX_DATA_LEN, 0);
                        printf("dst2 = %d\n", pmsg.msg.head.dst);
                        //printf("dst1 = %d\n", msg.head.dst);
                        //printf("M send scan require successfully, current slot = %d\n", info.current_slot);
                        ///*打开扫描响应定时器*/
                        //info.timerId_M[i] = timeSetEvent(TIMER_DELAY, 0, TimerCallback, SCAN_RES_TIMER, TIME_ONESHOT);
                    }
                    else
                    {
                        continue;
                    }
                }                
            }
            break;
        case FSM_ON://建链完成状态，判断当前时隙给哪个Z发数据
            if (info.current_slot == 0 || info.current_slot == 5)
            {
                /*信令时隙*/
            }
            else if (info.current_slot == 61)
            {
                /*测距时隙*/

            }
            else if (18 <= info.current_slot && info.current_slot <= 28)
            {
                /*M1_SEND_M2，M1_SNED_Z5*/
            }
            else
            {
                for (i = 1; i < FD_NUM; i++)
                {

                    index = schedule_inquire_index(i, info.current_slot);
                    if (index != -1)
                    {
                        /*发送数据帧*/
                        //dequeue(&info.thread_queue[DATA_SEND_THREAD], msg.data, &msg.len);
                        msg.data[0] = 5;
                        msg.len = 1;
                        generate_packet(info.device_info.node_id[index], info.device_info.node_id[MY_INDEX], LONG_FRAME, &msg);
                        psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                        send(FD[index].fd, &pmsg, MAX_DATA_LEN, 0);
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
        case FSM_INIT:
            break;
        case FSM_OFF:
            break;
        case FSM_WAN://正在建链状态，响应M的扫描询问帧
            if (info.current_slot != 59)
            {
                /*发送扫描响应帧*/
                dequeue(&info.thread_queue[DATA_SEND_THREAD], msg.data, &msg.len);
                generate_packet(info.device_info.node_id[0], info.device_info.node_id[MY_INDEX], SCAN, &msg);
                psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                send(FD[0].fd, &pmsg, MAX_DATA_LEN, 0);
                /*打开扫描回复定时器*/
                info.timerId = timeSetEvent(TIMER_DELAY, 0, TimerCallback, SCAN_CON_TIMER, TIME_ONESHOT);
            }

            break;
        case FSM_ON://建链完成状态，发送数据帧
            if ((31 <= info.current_slot && info.current_slot <= 34) && MY_INDEX == 1)
            {
                dequeue(&info.thread_queue[DATA_SEND_THREAD], msg.data, &msg.len);
                generate_packet(info.device_info.node_id[0], info.device_info.node_id[MY_INDEX], LONG_FRAME, &msg);
                psy_send(msg.len, &pmsg, &msg, info.current_antenna, info.device_info.node_role);
                send(FD[0].fd, &pmsg, MAX_DATA_LEN, 0);
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
        msg->head.send_time = info.str.base_time;
    }
    else
    {
        clock_gettime(CLOCK_REALTIME, &msg->head.send_time);
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
    }
    else//Z
    {
        msg->head.antenna_id = info.antenna_Z;
    }

    msg->len = msg->len + sizeof(head_t) + sizeof(int);//加上帧头长度
    /*位置信息待补充*/
}