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

        Sleep(3);

        /*下发*/
    }
}

/*
功能：主控线程数据队列的数据处理
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
                break;
            case START_GUN:
                if (msg.data[0] == START_GUN_RES)
                {
                    fsm_do(EVENT_WAIT_SIMULATE);
                }
                else
                {
                    printf("M not receive start_gun response\n");
                }
                break;
            case SCAN:
                if (msg.data[0] == SCAN_RES)
                {
                    index = inquire_slot(info.current_slot);
                    info.device_info.node_num++;
                    info.device_info.node_list = info.device_info.node_list | (1 << index);
                    info.scan_flag[index] = 1;
                    printf("list =  %d\n", info.device_info.node_list);
                    //if (info.device_info.node_num == FD_NUM)
                    //{
                    //    fsm_do(EVENT_WSN_SUCC);
                    //}
                    ///*扫描回复帧*/
                    //msg.data[0] = 3;
                    //msg.len = 1;
                    //enqueue(&info.thread_queue[DATA_SEND_THREAD_SCAN_CON], msg.data, msg.len);
                }
                else
                {
                    printf("M not receive scan response\n");
                }
                break;
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
                        info.str.start_time = msg.data[1];
                        printf("Z base time=%lld, %ld, start_time = %d\n", msg.head.send_time.tv_sec, msg.head.send_time.tv_nsec, info.str.start_time);
                        /*响应主机的发令枪帧*/
                        msg.data[0] = START_GUN_RES;
                        msg.len = 1;
                        generate_packet(info.device_info.node_id[0], info.device_info.node_id[MY_INDEX], START_GUN, &msg);
                        send(FD[0].fd, &msg, msg.len, 0);

                        /*同步仿真建链时间，将自身状态设置为fsm_wan*/
                        clock_gettime(CLOCK_REALTIME, &info.str.base_time);
                        while ((info.str.base_time.tv_sec * 1000000000 + info.str.base_time.tv_nsec) < (msg.head.send_time.tv_sec * 1000000000 + msg.head.send_time.tv_nsec + info.str.start_time * 1000000000))
                        {
                            clock_gettime(CLOCK_REALTIME, &info.str.base_time);
                        }
                        fsm_do(EVENT_WAIT_ACCESS);
                    }
                    else
                    {
                        printf("Z not receive start_gun require\n");
                    }
                    break;
                case SCAN:
                    if (msg.data[0] == SCAN_REQ)
                    {
                        /*关闭扫描询问定时器*/
                        timeKillEvent(info.timerId);
                        /*扫描响应帧*/
                        msg.data[0] = SCAN_RES;
                        msg.len = 1;
                        enqueue(&info.thread_queue[DATA_SEND_THREAD], msg.data, msg.len);
                    }
                    else if (msg.data[0] == SCAN_CON)
                    {
                        /*关闭扫描回复定时器*/
                        timeKillEvent(info.timerId);
                        fsm_do(EVENT_WAN_SUCC);
                    }
                    else
                    {
                        printf("Z not receive scan require\n");
                    }
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

