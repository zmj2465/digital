#include "rs485_recv_thread.h"
#include "xdmaDLL_public.h"

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
    int ret = 0;

    HANDLE recv_hdev;
    HANDLE send_hdev;
    char a[100];
    ret = open_devices(&recv_hdev, GENERIC_READ, a, (const char*)XDMA_FILE_C2H_0);
    ret = open_devices(&send_hdev, GENERIC_WRITE, a, (const char*)XDMA_FILE_H2C_0);

    while (1)
    {

    }
}


