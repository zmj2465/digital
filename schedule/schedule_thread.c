#include "schedule_thread.h"

#define SLOT_NUM 5

int slot_table[SLOT_NUM] = { 200,400,800,200,600 };


void* schedule_thread(void* arg)
{
    pthread_detach(pthread_self());

    set_thread_priority();

    /*等待开始信号*/
    sleep(10);

    int i = 0;

    while (1)
    {
        /*delay*/
        udelay(1);
        /*发送信号*/
        sem_post(&info.send_semaphore);
    }
}


