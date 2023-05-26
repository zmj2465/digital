#include "fddi_thread.h"


void* fddi_thread(void* arg)
{
    pthread_detach(pthread_self());

    fddi_thread_init();

    char data[MAX_DATA_LEN];
    int len;

    while (1)
    {
        /*���չ�����Ϣ*/

        /*����master*/
        enqueue(&info.thread_queue[MASTER_THREAD], data, len);
    }
}


void fddi_thread_init()
{
    int lfd;
    int ret = 0;

    //��������socket
    lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //�󶨱���ip��ַ���˿ں�
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, info.fddi_ip, (void*)&addr.sin_addr); //**ip**
    addr.sin_port = htons(info.fddi_port); //**port**
    ret = bind(lfd, (struct sockaddr*)&addr, sizeof addr);

    //��ʼ����
    listen(lfd, SOMAXCONN);

    info.fddi_system.addr_len = sizeof(info.fddi_system.addr); //**
    info.fddi_system.fd = accept(lfd, (struct sockaddr*)&(info.fddi_system.addr), &(info.fddi_system.addr_len)); //**

    printf("fddi_system connect success %d\n", info.fddi_system.fd);
}