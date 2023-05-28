#include "master_thread.h"


void* master_thread(void* arg)
{
    pthread_detach(pthread_self());

    char data[MAX_DATA_LEN];
    int len;

    while (1)
    {
        /*接收消息*/
        dequeue(&info.thread_queue[MASTER_THREAD], &data, &len);

        /*数据分析*/
        sleep(3);

        /*下发*/

    }
}