#include "rs485_recv_thread.h"

#define MSGLEN   4
#define FLAG_LEN 1
#define HEAD_LEN 3
#define CC_LEN   100


void msg_proc(char* get)
{
    
}


void* rs_485_recv_thread(void* arg)
{
    pthread_detach(pthread_self());

    char get[MAX_DATA_LEN];
    rs485_info_t* p = (rs485_info_t*)get;
    while (1)
    {
        switch (p->type)
        {
            case SP:
                enqueue_no_block(&info.thread_queue[DATA_SEND_THREAD], p->sp, SP_LEN);
                break;
        }
    }
}


