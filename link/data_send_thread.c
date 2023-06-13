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
    msg_t msg;
    int i;
    int index;
    memset(&msg, 0, sizeof(msg_t));
    /*主机M*/
    if (MY_INDEX == 0)
    {
        switch (fsm_status)
        {
        case FSM_WSN://正在建链状态，按时隙进行扫描询问和数据发送
            if (info.current_slot == 0 || info.current_slot == 5)//信令时隙
            {
                //dequeue(&info.thread_queue[DATA_SEND_THREAD_SCAN], msg.data, &msg.len);
                for (i = 1; i < FD_NUM; i++)
                {
                    if (inquire_index(i) == -1)//对应终端Z不在网
                    {
                        msg.data[0] = SCAN_REQ;
                        msg.len = 1;
                        /*发送扫描询问帧*/
                        generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], SCAN, &msg);
                        send(FD[i].fd, &msg, msg.len, 0);
                        ///*打开扫描响应定时器*/
                        //info.timerId_M[i] = timeSetEvent(TIMER_DELAY, 0, TimerCallback, SCAN_RES_TIMER, TIME_ONESHOT);
                    }
                }
                //fsm_do(EVENT_WSN);
            }
            else if (info.current_slot == 61)
            {
                /*测距时隙*/
                
            }
            else if(17 < info.current_slot && info.current_slot < 29)
            {
                /*M1_SEND_M2，M1_SNED_Z5*/
              
            }
            else
            {
                for (i = 1; i < FD_NUM; i++)
                {
                    
                    index = schedule_inquire_index(i, info.current_slot);
                    if (index)
                    {
                        if (info.scan_flag[index])
                        {
                            printf("M send Z%d scan_con success\n" , index);
                            msg.data[0] = SCAN_CON;
                            msg.len = 1;
                            /*发送扫描回复帧*/
                            //dequeue(&info.thread_queue[DATA_SEND_THREAD_SCAN_CON], msg.data, &msg.len);
                            generate_packet(info.device_info.node_id[index], info.device_info.node_id[MY_INDEX], SCAN, &msg);
                            send(FD[index].fd, &msg, msg.len, 0);
                            info.scan_flag[index] = 0;
                            return 0;
                            
                        }
                        else
                        {
                            /*发送数据帧*/
                            //dequeue(&info.thread_queue[DATA_SEND_THREAD], msg.data, &msg.len);
                            //msg.data[0] = SCAN_CON;
                            //msg.len = 1;
                            generate_packet(info.device_info.node_id[index], info.device_info.node_id[MY_INDEX], SHORT_FRAME, &msg);
                            send(FD[index].fd, &msg, msg.len, 0);
                            return 0;
                        }

                    }
                }
                //dequeue(&info.thread_queue[DATA_SEND_THREAD_SCAN], msg.data, &msg.len);
                for (i = 1; i < FD_NUM; i++)
                {
                    if (inquire_index(i) == -1)//对应终端Z不在网
                    {
                        msg.data[0] = SCAN_REQ;
                        msg.len = 1;
                        /*发送扫描询问帧*/
                        generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], SCAN, &msg);
                        send(FD[i].fd, &msg, msg.len, 0);
                        ///*打开扫描响应定时器*/
                        //info.timerId_M[i] = timeSetEvent(TIMER_DELAY, 0, TimerCallback, SCAN_RES_TIMER, TIME_ONESHOT);
                    }
                }
                //fsm_do(EVENT_WSN);
            }
            break;
        case FSM_ON://建链完成状态，判断当前时隙给哪个Z发数据
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
        case FSM_WAN://处于正在建链状态，按照自行规划的时隙响应M的扫描询问
            /*发送扫描响应帧*/
            dequeue(&info.thread_queue[DATA_SEND_THREAD], msg.data, &msg.len);
            generate_packet(info.device_info.node_id[0], info.device_info.node_id[MY_INDEX], SCAN, &msg);
            send(FD[0].fd, &msg, msg.len, 0);
            /*打开扫描回复定时器*/
            info.timerId = timeSetEvent(TIMER_DELAY, 0, TimerCallback, SCAN_CON_TIMER, TIME_ONESHOT);
            break;
        case FSM_ON://处于建链完成状态，判断当前时隙是给Z或M发数据

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
    msg->head.dst = dst;
    msg->head.src = src;
    msg->head.type = type;
    msg->head.antenna_id = info.current_antenna;

    if (type == START_GUN && MY_INDEX == 0)
    {
        msg->head.send_time = info.str.base_time;
    }
    else
    {
        clock_gettime(CLOCK_REALTIME, &msg->head.send_time);
    }

    msg->len = msg->len + sizeof(head_t) + sizeof(int);//加上帧头长度
    /*位置信息待补充*/
}