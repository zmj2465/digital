#include "fddi_thread.h"
#include "physical_simulation.h"

int lfd;

//generate_key_event(0); 上电
//generate_key_event(1); 自检
//generate_key_event(2); 加注
//generate_key_event(3); 预分离
//generate_key_event(4); 分离
//generate_key_event(5); 建链

void* fddi_thread(void* arg)
{
    pthread_detach(pthread_self());
    int h = 0;
    int flag = 0;
    fddi_thread_init();

    char data[MAX_DATA_LEN];
    int len;

    len = recv(info.fddi_system.fd, data, 400, 0);
    prepare_simulation = 1;
    display_state.mode = SIM_MODE;

    while (1)
    {
        len = recv(info.fddi_system.fd, data, 400, 0);
        if (len <= 0)
        {
            display_state.mode = NO_MODE;
            info.fddi_system.fd = accept(lfd, (struct sockaddr*)&(info.fddi_system.addr), &(info.fddi_system.addr_len));
            printf("fddi_system connect success %d\n", info.fddi_system.fd);
            display_state.mode = SIM_MODE;
            h = 0;
        }
        else
        {
            memcpy(overall_fddi_info, data, sizeof(fddi_info_t) * 5);
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


void send_start()
{
    char buff[100];
    int ret = send(info.fddi_system.fd, &buff, 100, 0);
    printf("ret==%d\n", ret);
}

void wait_for_fddi()
{
    while (info.fddi_system.fd == 0);
}



