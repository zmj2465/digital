#include "fddi_thread.h"
#include "physical_simulation.h"

int lfd;

void* fddi_thread(void* arg)
{
    pthread_detach(pthread_self());

    fddi_thread_init();

    char data[MAX_DATA_LEN];
    int len;

    len = recv(info.fddi_system.fd, data, 200, 0);
    prepare_simulation = 1;
    while (1)
    {
        len = recv(info.fddi_system.fd, data, 200, 0);
        if (len <= 0)
        {
            info.fddi_system.fd = accept(lfd, (struct sockaddr*)&(info.fddi_system.addr), &(info.fddi_system.addr_len));
            printf("fddi_system connect success %d\n", info.fddi_system.fd);
        }
        else
        {
            char temp[40];
            memcpy(&fddi_info, data, sizeof(fddi_info_t));
            memcpy(&overall_fddi_info[1], data + sizeof(fddi_info_t), sizeof(fddi_info_t));
            memcpy(temp, data + 2 * sizeof(fddi_info_t), 24);
            temp[24] = 0;
            printf("%s\n", temp);
            send_display_msg();
        }
    }
}


void fddi_thread_init()
{
    int ret = 0;

    //创建侦听socket
    lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //设置端口复用
    int opt = 1;
    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) == -1) plog("setsockopt error");

    //绑定本机ip地址、端口号
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, info.fddi_ip, (void*)&addr.sin_addr); //**ip**
    addr.sin_port = htons(info.fddi_port); //**port**
    ret = bind(lfd, (struct sockaddr*)&addr, sizeof addr);


    //开始监听
    listen(lfd, SOMAXCONN);

    info.fddi_system.addr_len = sizeof(info.fddi_system.addr); //**
    info.fddi_system.fd = accept(lfd, (struct sockaddr*)&(info.fddi_system.addr), &(info.fddi_system.addr_len)); //**

    printf("fddi_system connect success %d\n", info.fddi_system.fd);
}


