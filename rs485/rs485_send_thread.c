#include "rs485_send_thread.h"
#include <string.h>


void* rs_485_send_thread(void* arg)
{
    pthread_detach(pthread_self());

    char data[MAX_DATA_LEN];
    int len;

    while (1)
    {
        /*��ȡ��Ϣ*/
        dequeue(&info.thread_queue[RS485_SEND_THREAD], &data, &len);

        /*����rs485*/


    }

}