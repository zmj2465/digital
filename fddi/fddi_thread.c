#include "fddi_thread.h"
#include "physical_simulation.h"

void* fddi_thread(void* arg)
{
    pthread_detach(pthread_self());

    while (1)
    {
        generate_show_msg();
        Sleep(1000);
    }

    fddi_thread_init();

    char data[MAX_DATA_LEN];
    int len;

    fddi_info_t* p = (fddi_info_t*)data;

    

    while (1)
    {
        /*接收光纤信息*/
        len=recv(info.fddi_system.fd, data, MAX_DATA_LEN, 0);
        prepare_simulation = 1;
        printf("get fddi info len=%d\n", len);
        fddi_info.pos.x = p->pos.x;
        fddi_info.pos.y = p->pos.y;
        fddi_info.pos.z = p->pos.z;
        fddi_info.q.q0 = p->q.q0;
        fddi_info.q.q1 = p->q.q1;
        fddi_info.q.q2 = p->q.q2;
        fddi_info.q.q3 = p->q.q3;
        printf("a=%x,b=%x,d=%x\n", data[0], data[1], data[2]);
        printf("x=%g\ny=%g\nz=%g\nq0=%g\nq1=%g\nq2=%g\nq3=%g\n", p->pos.x, p->pos.y, p->pos.z, p->q.q0, p->q.q1, p->q.q2, p->q.q3);

        generate_show_msg();
        /*送往master*/
        //enqueue(&info.thread_queue[MASTER_THREAD], data, len);
    }
}


void fddi_thread_init()
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
    inet_pton(AF_INET, info.fddi_ip, (void*)&addr.sin_addr); //**ip**
    addr.sin_port = htons(info.fddi_port); //**port**
    ret = bind(lfd, (struct sockaddr*)&addr, sizeof addr);

    printf("fddi_thread_init ret=%d\n", ret);

    //开始监听
    listen(lfd, SOMAXCONN);

    info.fddi_system.addr_len = sizeof(info.fddi_system.addr); //**
    info.fddi_system.fd = accept(lfd, (struct sockaddr*)&(info.fddi_system.addr), &(info.fddi_system.addr_len)); //**

    printf("fddi_system connect success %d\n", info.fddi_system.fd);
}


void generate_show_msg()
{
    int ret = 0;
    show_msg.type = 0;
    show_msg.len = 4 + sizeof(display_t);
    show_msg.display_info.serial_number = display_state.seq++;

    //存储
    ret = fwrite(&show_msg, sizeof(char), show_msg.len, info.data_file);
}