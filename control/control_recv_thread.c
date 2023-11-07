#include "control_recv_thread.h"

void* control_recv_thread(void* arg)
{
    pthread_detach(pthread_self());

    /*��̫������*/
    control_recv_thread_init();

    char data[MAX_DATA_LEN];
    int len;

    while (1)
    {
        /*���ݽ���*/
        
        //read

        /*����master*/
        enqueue(&info.thread_queue[MASTER_THREAD], data, len);
    }
}



void control_recv_thread_init()
{
    int lfd;
    int ret = 0;

    //��������socket
    lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //���ö˿ڸ���
    int opt = 1;
    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) == -1) plog("setsockopt error");

    //�󶨱���ip��ַ���˿ں�
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, info.control_ip, (void*)&addr.sin_addr); //**ip**
    addr.sin_port = htons(info.control_port); //**port**
    ret = bind(lfd, (struct sockaddr*)&addr, sizeof addr); //��ip�Ͷ˿�

    //��ʼ����
    listen(lfd, SOMAXCONN);

    info.control_system.addr_len = sizeof(info.control_system.addr);
    info.control_system.fd = accept(lfd, (struct sockaddr*)&(info.control_system.addr), &(info.control_system.addr_len));//������������

    printf("control_system connect success %d\n", info.control_system.fd);

}

