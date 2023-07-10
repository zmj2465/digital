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



    while (1)
    {
        /*接收信息*/
        //read

        msg_proc(get);
    }
}

void generate_show_msg()
{
    show_msg.type = 0;
    show_msg.len = 4+sizeof(display_t);
    show_msg.display_info.serial_number = display_state.seq++;
}

