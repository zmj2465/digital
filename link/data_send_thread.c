#include "data_send_thread.h"


/*
功能：发送数据线程
参数：无
返回值：无
*/
void* data_send_thread(void* arg)
{
    pthread_detach(pthread_self());

    msg_t msg;

    while (1)
    {
        sem_wait(&info.send_semaphore);

        /*出队*/
        dequeue(&info.thread_queue[DATA_SEND_THREAD], msg.data, &msg.len);

        /*组包发送*/
        data_send_proc(&msg);



    }
}

/*
功能：数据发送的处理
参数：无
返回值：无
*/
int data_send_proc(msg_t* smsg)
{
    int i = 0;
    if (MY_INDEX == 0)//主机
    {
        switch (fsm_status)
        {
        case FSM_INIT://初始化未开始建链，给各个从机发送配置文件

            break;
        case FSM_OFF://发送信令枪启动仿真建链
            for (i = 1; i < FD_NUM; i++)
            {
                generate_packet(info.device_info[i].node_id, info.device_info[MY_INDEX].node_id, START_GUN, smsg);
                send(FD[i].fd, smsg, sizeof(msg_t), 0);
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
    else//从机
    {
        switch (fsm_status)
        {
        case FSM_INIT:

            break;
        case FSM_OFF:

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
功能：组包生成数据帧
参数：目的地址，源地址，当前系统时间，帧类型，位置信息等
返回值：无
*/
void generate_packet(uint8_t dst, uint8_t src, uint8_t type, msg_t* msg)
{
    //msg_t frm;
    //memset(&frm, 0, sizeof(msg_t));
    //frm.head.dst = dst;
    //frm.head.src = src;
    //frm.head.type = type;
    //clock_gettime(CLOCK_MONOTONIC, &frm.head.sbt.base_time);
    //if (type == START_GUN)
    //{
    //    frm.head.sbt.start_time = 10; /*仿真开始时间，暂定10s后*/
    //}
    //memcpy(frm.data, msg->data, msg->len);

    msg->head.dst = dst;
    msg->head.src = src;
    msg->head.type = type;
    clock_gettime(CLOCK_MONOTONIC, &msg->head.sbt.base_time);
    if (type == START_GUN)
    {
        msg->head.sbt.start_time = 10;/*仿真开始时间，暂定10s后*/
    }
    /*位置信息等待补充*/
}