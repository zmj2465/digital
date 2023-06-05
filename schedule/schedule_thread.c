#include "schedule_thread.h"

#define SLOT_NUM 5

int slot_table[SLOT_NUM] = { 200,400,800,200,600 };



void* schedule_thread(void* arg)
{
    pthread_detach(pthread_self());

    set_thread_priority();
    /*等待开始信号*/
    Sleep(10);

    while (1)
    {

        /*高精度定时器，单位：微秒*/
        udelay(100000);
        /*发送信号*/
        sem_post(&info.send_semaphore);

    }
}

/*
功能：时隙调度
参数：无
返回值：0表示成功
*/
int schedule_slot(void)
{
  
  

    
    return 0;
}

/*
功能：定时器处理
参数：无
返回值：无
*/
static void timer_handle(int para)
{
    static int count = 0;
    if (++count == 5)
    {
        receive_slot();
    }
    else
    {
        send_slot();
    }
}

/*
功能：发送时隙
参数：无
返回值：无
*/
void send_slot(void)
{
    printf("send slot\n");
}

/*
功能：接收时隙
参数：无
返回值：无
*/
void receive_slot(void)
{
    printf("receive slot\n");
}