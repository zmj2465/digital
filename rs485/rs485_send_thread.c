#include "rs485_send_thread.h"
#include "rs485_recv_thread.h"
#include <string.h>


void* rs_485_send_thread(void* arg)
{
    pthread_detach(pthread_self());

    char data[MAX_DATA_LEN];
    int len;


    while (1)
    {
        //enqueue(&info.thread_queue[RS485_RECV_THREAD], data, 565);
        Sleep(100);
    }

}