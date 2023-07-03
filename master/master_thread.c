#include "master_thread.h"

/*
功能：主控线程
参数：无
返回值：无
*/
void* master_thread(void* arg)
{
    pthread_detach(pthread_self());

    while (1)
    {
        /*控制队列处理*/
        
        
        /*数据队列处理*/
        master_data_proc();

    }
}

/*
功能：主控线程数据队列的处理
参数：无
返回值：无
*/
int master_data_proc(void)
{
    int index;
    msg_t msg;
    dequeue(&info.thread_queue[MASTER_THREAD_DATA], &msg, &msg.len);

    /*解包*/
    msg.len = msg.len - sizeof(head_t) - sizeof(int);

    if (msg.head.dst == info.device_info.node_id[MY_INDEX])
    {
        /*主机*/
        if (MY_INDEX == 0)
        {
            switch (msg.head.type)
            {
            case SHORT_FRAME:
                break;
            case LONG_FRAME:
                index = inquire_address(msg.head.src);

#ifdef _WIN32
                /*M的数据帧定时器*/
                timeKillEvent(info.timerId_M[index]);
                info.timerId_M[index] = timeSetEvent(TIMER_DELAY, 0, TimerCallback, index, TIME_ONESHOT);
#endif

                printf("M receive Z%d data frame, current slot = %d.%d\n", index, info.current_time_frame, info.current_slot);
                break;
            case START_GUN:
                if (msg.data[0] == START_GUN_RES)
                {
                    fsm_do(EVENT_WAIT_SIMULATE);
                }
                else
                {
                    printf("M not receive start-gun response\n");
                }
                break;
            case SCAN:
                if (msg.data[0] == SCAN_RES)
                {
                    index = inquire_address(msg.head.src);
                    info.device_info.node_num++;
                    info.device_info.node_list = info.device_info.node_list | (1 << index);
                    info.scan_flag_M[index] = 1;
                    printf("M receive Z%d scan response, list = %d, current slot = %d.%d\n", index, info.device_info.node_list, info.current_time_frame, info.current_slot);
                }
                else
                {
                    printf("M not receive scan response\n");
                }
                break;
            case DISTANCE:
                if (msg.data[0] == DISTANCE_Z)
                {
                    index = inquire_address(msg.head.src);
                    info.distance_flag_M[index] = 1;
                    printf("M receive Z%d distance frame, current slot = %d.%d\n", index, info.current_time_frame, info.current_slot);
                }
                else
                {
                    printf("M not receive Z distance frame\n");
                }
            default:
                break;
            }
        }
        /*从机*/
        else
        {
            switch (msg.head.type)
            {
                case START_GUN:
                    if (msg.data[0] == START_GUN_REQ)
                    {
                        info.str.base_time = msg.head.send_time;
                        info.str.start_time = msg.data[1];
                        printf("Z%d base time=%lld, %ld, start_time = %d\n", MY_INDEX, info.str.base_time.tv_sec, info.str.base_time.tv_nsec, info.str.start_time);
                        /*响应主机的发令枪帧*/
                        msg.data[0] = START_GUN_RES;
                        msg.len = 1;
                        generate_packet(info.device_info.node_id[0], info.device_info.node_id[MY_INDEX], START_GUN, &msg);
                        send(FD[0].fd, &msg, msg.len, 0);
                        fsm_do(EVENT_WAIT_ACCESS);
                    }
                    else
                    {
                        printf("Z not receive start-gun require\n");
                    }
                    break;
                case SCAN:
                    if (msg.data[0] == SCAN_REQ)
                    {
#ifdef _WIN32
                        /*关闭扫描询问定时器*/
                        timeKillEvent(info.timerId);
#endif
                        /*扫描响应帧*/
                        info.scan_flag_Z = 1;
                    }
                    else if (msg.data[0] == SCAN_CON)
                    {
#ifdef _WIN32
                        /*关闭扫描回复定时器*/
                        timeKillEvent(info.timerId);
#endif
                        fsm_do(EVENT_WAN_SUCC);
                    }
                    else
                    {
                        printf("Z not receive scan\n");
                    }
                    break;
                case LONG_FRAME:
#ifdef _WIN32
                    /*Z的数据帧定时器*/
                    timeKillEvent(info.timerId_Z);
                    info.timerId_Z = timeSetEvent(TIMER_DELAY, 0, TimerCallback, Z_DATA_TIMER, TIME_ONESHOT);
#endif
                    //printf("Z%d receive M data frame, current slot = %d.%d\n", MY_INDEX, info.current_time_frame, info.current_slot);       
                    break;
                default:
                    break;
            }

        }
    }
    else
    {
        printf("dst uncorrectly, master thread receive failed\n");
    }

    return 0;
}

/*
功能：主控线程控制队列的处理
参数：无
返回值：无
*/
int master_control_proc(void)
{

    return 0;
}