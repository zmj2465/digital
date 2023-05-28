#include "data_send_thread.h"
#include <math.h>



void* data_send_thread(void* arg)
{
    pthread_detach(pthread_self());

    int i = 0;


    while (1)
    {
        sem_wait(&info.send_semaphore);

        /*组相应数据包*/
        //dequeue

        /*发送*/
        

    }
}