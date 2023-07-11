#include "link_control_thread.h"

void* link_control_thread(void* arg)
{
    pthread_detach(pthread_self());

	link_init();

	sem_post(&info.thread_create_semaphore);

	link_info_print();

	//init_complete_judge();

	relink();

}


void link_init()
{
	int ret = 0;

	//创建侦听socket
	LFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	//设置端口复用
	int opt = 1;
	if (setsockopt(LFD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) == -1) printf("setsockopt error");

	//绑定本机ip地址、端口号
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	inet_pton(AF_INET, info.ip, (void*)&addr.sin_addr);
	addr.sin_port = htons(info.port);
	ret = bind(LFD, (struct sockaddr*)&addr, sizeof addr);

	//开始监听
	listen(LFD, SOMAXCONN);

	int i = 0;

	/*本机作为服务端 等待比自身索引号大的设备连接*/
	for (i = MY_INDEX + 1; i < info.simulated_link_num; i++)
	{
		printf("wait for connect\n");
		struct sockaddr_in temp_addr;
		FD[i].addr_len = sizeof(FD[i].addr);
		int fd= accept(LFD, (struct sockaddr*)&temp_addr, &(FD[i].addr_len));
		int j;

		char buff[1024];
		recv(fd, buff, 1024, 0);
		int x = *(int*)buff;

		FD[x].fd = fd;
		memcpy(&FD[x].addr, &temp_addr, sizeof(struct sockaddr_in));
		printf("(device)%d (ip)%s (fd)%d is connected\n", x, inet_ntoa(FD[x].addr.sin_addr), FD[x].fd);
	/*	int k;
		for (k = 0; k < 1000; k++) {
			recv(fd, buff, 1024, 0);
			uint64_t b = *(uint64_t*)buff;
			uint64_t c;
			c = my_get_time();
			printf("b = %lld , c = %lld\n", b,c);
			printf("%lld us\n", (c - b) / 1000);
		}
		while (1);*/
	}

	/*本机作为客户端 连上比自身索引号小的设备*/
	for (i = MY_INDEX - 1; i >= 0; i--)
	{
		FD[i].fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		memset(&FD[i].addr, 0, sizeof(FD[i].addr));
		FD[i].addr.sin_family = AF_INET;
		FD[i].addr.sin_port = htons(FD[i].port);
		inet_pton(AF_INET, FD[i].ip, (void*)&FD[i].addr.sin_addr);

		printf("is connecting to (device)%d (ip)%s (fd)%d (port)%d\n", i, FD[i].ip, FD[i].fd, FD[i].port);
		while (0 != connect(FD[i].fd, (struct sockaddr*)&FD[i].addr, sizeof(FD[i].addr)))
		{
			perror("link connect");
		}

		send(FD[i].fd, &MY_INDEX, sizeof(int), 0);
		printf("send: %d\n", MY_INDEX);


		//uint8_t aaa[1024];
		//printf("connect device %d ip %s succeed\n", i, inet_ntoa(FD[i].addr.sin_addr));
		//int k;
		//for (k = 0; k < 1000; k++) {
		//	*(uint64_t*)aaa =my_get_time();
		//	//uint64_t a = my_get_time();
		//	send(FD[i].fd, &aaa, 1024, 0);
		//	//printf("a = %lld\n", a);
		//	//udelay(800);
		//	Sleep(1000);
		//}
		//while (1);

	}

	printf("all link complete\n");

	return;
}


void link_complete_boardcast()
{
	int i = 0;
	start_boardcast_t boardcast_msg;

	clock_gettime(CLOCK_REALTIME, &boardcast_msg.base_time);
	boardcast_msg.start_time = 10;

	if (MY_INDEX == 0)
	{
		for (i = 1; i < info.simulated_link_num; i++)
		{
			send(FD[i].fd, &boardcast_msg, sizeof(start_boardcast_t), 0);
			printf("base time=%lld,%ld start time=%d\n", boardcast_msg.base_time.tv_sec, boardcast_msg.base_time.tv_nsec, boardcast_msg.start_time);
		}
	}
}

void init_complete_judge()
{
	while (info.control_system.fd == 0 || info.display_system.fd == 0 || info.fddi_system.fd == 0)
	{
		sleep(5);
	}
}


void link_info_print()
{
	int i;
	for (i = 0; i < info.simulated_link_num; i++)
	{
		printf("(device)%d (ip)%s (aip)%s (fd)%d\n", i, FD[i].ip, inet_ntoa(FD[i].addr.sin_addr), FD[i].fd);
	}
}


void relink()
{
	msg_t msg;
	int i = 0;
	while (1)
	{
		dequeue(&info.thread_queue[LINK_CONTROL_THREAD],&msg, &msg.len);
		i = msg.data[0];
		printf("relink to node %d\n", i);

		/*本机作为服务端 等待比自身索引号大的设备连接*/
		if (MY_INDEX < i)
		{
			printf("wait for connect\n");
			struct sockaddr_in temp_addr;
			FD[i].addr_len = sizeof(FD[i].addr);
			int fd = accept(LFD, (struct sockaddr*)&temp_addr, &(FD[i].addr_len));
			int j;
			char buff[1024];
			recv(fd, buff, 1024, 0);
			int x = *(int*)buff;
			FD[x].fd = fd;
			memcpy(&FD[x].addr, &temp_addr, sizeof(struct sockaddr_in));
			printf("(device)%d (ip)%s (fd)%d is connected\n", x, inet_ntoa(FD[x].addr.sin_addr), FD[x].fd);
		}
		/*本机作为客户端 连上比自身索引号小的设备*/
		else
		{
			FD[i].fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			memset(&FD[i].addr, 0, sizeof(FD[i].addr));
			FD[i].addr.sin_family = AF_INET;
			FD[i].addr.sin_port = htons(FD[i].port);
			inet_pton(AF_INET, FD[i].ip, (void*)&FD[i].addr.sin_addr);

			printf("is connecting to (device)%d (ip)%s (fd)%d (port)%d\n", i, FD[i].ip, FD[i].fd, FD[i].port);
			while (0 != connect(FD[i].fd, (struct sockaddr*)&FD[i].addr, sizeof(FD[i].addr)))
			{
				perror("link connect");
			}
			send(FD[i].fd, &MY_INDEX, sizeof(int), 0);
			printf("send: %d\n", MY_INDEX);
		}
	}
}
