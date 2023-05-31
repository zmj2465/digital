#include "master_thread.h"

/*
功能：主控线程
参数：无
返回值：无
*/
void* master_thread(void* arg)
{
    pthread_detach(pthread_self());

    msg_t msg;

    while (1)
    {
        /*接收消息*/
        dequeue(&info.thread_queue[MASTER_THREAD], msg.data, &msg.len);
        /*数据队列处理*/
        master_proc();
        sleep(3);
        /*下发*/
    }
}

/*
功能：主控线程数据队列的数据处理
参数：无
返回值：无
*/
int master_proc(void)
{
    msg_t msg;
    dequeue(&info.thread_queue[MASTER_THREAD_DATA], &msg, &msg.len);
    switch (msg.head.type)
    {
    case SLEF_TEST:

        break;
    case SLEF_TEST_RESULT:

        break;
    case PARAMETER_LOAD:

        break;
    case SHORT_FRAME:

        break;
    case LONG_FRAME:

        break;
    case START_GUN:
        if (MY_INDEX == 0)//主机
        {
        }
        else//从机
        {
            /*计算好系统时间，将自身状态设置为fsm_wan*/
            /*send*/
        }
        break;
    default:
        break;
    }
    return 0;
}