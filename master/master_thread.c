#include "master_thread.h"


void* master_thread(void* arg)
{
    pthread_detach(pthread_self());

    char data[MAX_DATA_LEN];
    int len;

    while (1)
    {
        /*������Ϣ*/
        dequeue(&info.thread_queue[MASTER_THREAD], &data, &len);

        /*���ݷ���*/
        sleep(3);

        /*�·�*/

    }
}