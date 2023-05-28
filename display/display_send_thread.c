#include "display_send_thread.h"


void* display_send_thread(void* arg)
{
    pthread_detach(pthread_self());

    ///*以太网连接*/
    display_send_thread_init();

    //char data[MAX_DATA_LEN];
    //int len;
    //while (1)
    //{
    //    /*数据接收*/
    //    dequeue(&info.thread_queue[DISPLAY_SEND_THREAD], &data, &len);
    //}

    char* sharedData = (char*)info.lpSharedMem;
    int count = 0;
    char test[] = { 69,70,71,72,73,74,75};
    while (1) {
        // 将数据写入共享内存
        memcpy(sharedData+info.mem_ptr, test, sizeof(test));
        info.mem_ptr += sizeof(test);

        printf("write to mem %d ok\n", info.mem_ptr);
        sleep(20);  // 假设每秒写入一次数据
    }

}

void display_send_thread_init()
{
    int lfd;
    int ret = 0;

    //创建侦听socket
    lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //设置端口复用
    int opt = 1;
    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) == -1) printf("setsockopt error");

    //绑定本机ip地址、端口号
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, info.display_ip, (void*)&addr.sin_addr); //**ip**
    addr.sin_port = htons(info.display_port); //**port**
    ret = bind(lfd, (struct sockaddr*)&addr, sizeof addr);

    //开始监听
    listen(lfd, SOMAXCONN);

    info.display_system.addr_len = sizeof(info.display_system.addr); //**
    info.display_system.fd = accept(lfd, (struct sockaddr*)&(info.display_system.addr), &(info.display_system.addr_len)); //**

    printf("display_system connect success %d\n", info.display_system.fd);

}