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
    dequeue(&info.thread_queue[DATA_SEND_THREAD], msg.data, &msg.len);

    /*主机M*/
    if (MY_INDEX == 0)
    {
        switch (fsm_status)
        {
        case FSM_INIT:
            break;
        case FSM_OFF://发送信令枪启动仿真建链
            clock_gettime(CLOCK_MONOTONIC, &info.str.base_time);
            info.str.start_time = start_gun_time;

            for (i = 1; i < FD_NUM; i++)
            {
                generate_packet(info.device_info[i].node_id, info.device_info[MY_INDEX].node_id, START_GUN, &msg);
                send(FD[i].fd, &msg, msg.len, 0);
            }
            break;
        case FSM_WSN://处于正在建链状态，按照自行规划的时隙进行扫描询问和数据发送

            break;
        case FSM_ON://处于建链完成状态，判断当前时隙给哪个Z发数据

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
            generate_packet(info.device_info[0].node_id, info.device_info[MY_INDEX].node_id, START_GUN, &msg);
            send(FD[0].fd, &msg, msg.len, 0);
            break;
        case FSM_WAN://处于正在建链状态，按照自行规划的时隙响应M的扫描询问

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

    if (type == START_GUN && MY_INDEX == 0)
    {
        //msg->head.sbt.base_time = info.str.base_time;
        //msg->head.sbt.start_time = info.str.start_time;
        msg->head.send_time = info.str.base_time;
    }
    else
    {
        //clock_gettime(CLOCK_MONOTONIC, &msg->head.sbt.base_time);
        //msg->head.sbt.start_time = 0;
        clock_gettime(CLOCK_MONOTONIC, &msg->head.send_time);
    }

    msg->len = msg->len + sizeof(head_t) + sizeof(int);//加上帧头长度
    /*位置信息待补充*/
}