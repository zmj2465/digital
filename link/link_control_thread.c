#include "link_control_thread.h"

void* link_control_thread(void* arg)
{
    pthread_detach(pthread_self());

	link_init();

	sem_post(&info.thread_create_semaphore);

	link_info_print();

	init_complete_judge();

	//link_complete_boardcast();

}

void configure_init()
{

	int ret = 0;
	int i;
	FILE* file;
	char line[IP_LEN];
	MY_INDEX = -1;

	file = fopen(PRIVATE_FILE, "r");
	if (file == NULL) {
		printf("无法打开文件。\n");
		return 1;
	}

	fgets(line, sizeof(line), file);
	sscanf(line, "%s", info.ip);

	fclose(file);

	for (i = 0; i < info.simulated_link_num; i++)
	{
		if (strcmp(info.ip, FD[i].ip) == 0)
		{
			MY_INDEX = i;
			break;
		}
	}

	printf("MY_INDEX=%d\n", MY_INDEX);
	
	//if (MY_INDEX == 0)/*主机*/
	//{
	//	/*根据配置文件连上各从机并发送配置文件*/
	//	for (i = 1; i < MAX_DEVICE; i++)
	//	{
	//		FD[i].fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//		memset(&FD[i].addr, 0, sizeof(FD[i].addr));
	//		FD[i].addr.sin_family = AF_INET;
	//		FD[i].addr.sin_port = htons(PORT);
	//		inet_pton(AF_INET, "192.168.3.15", (void*)&FD[i].addr.sin_addr);
	//		//FD[i].addr.sin_addr.s_addr = inet_addr("192.168.3.15");
	//		while (0 != connect(FD[i].fd, (struct sockaddr*)&FD[i].addr, sizeof(FD[i].addr)));
	//		printf("send config info to %d\n", i);
	//		ret = send(FD[i].fd, "configure info\n", 16, 0);
	//		close(FD[i].fd);
	//	}
	//}
	//else/*从机*/
	//{
	//	//*等待主机发送配置文件*/
	//	int lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	//	struct sockaddr_in addr;
	//	addr.sin_family = AF_INET;
	//	//FD[i].addr.sin_addr.s_addr = inet_addr("192.168.3.15");
	//	inet_pton(AF_INET, "192.168.3.15", (void*)&addr.sin_addr);
	//	addr.sin_port = htons(PORT);
	//	ret = bind(lfd, (struct sockaddr*)&addr, sizeof addr);
	//	listen(lfd, SOMAXCONN);
	//	FD[0].addr_len = sizeof(FD[0].addr);
	//	FD[0].fd = accept(lfd, (struct sockaddr*)&(FD[0].addr), &(FD[0].addr_len));
	//	ret = recv(FD[0].fd, FD[0].recvBuffer, MAX_DATA_LEN, 0);
	//	printf("get config info %s", FD[0].recvBuffer);
	//	close(lfd);
	//	close(FD[0].fd);
	//}


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
	addr.sin_port = htons(info.communication_port);
	ret = bind(LFD, (struct sockaddr*)&addr, sizeof addr);

	//开始监听
	listen(LFD, SOMAXCONN);

	int i = 0;

	/*本机作为服务端 等待比自身索引号大的设备连接*/
	for (i = MY_INDEX + 1; i < info.simulated_link_num; i++)
	{
		printf("wait for connect\n", i);
		struct sockaddr_in temp_addr;
		FD[i].addr_len = sizeof(FD[i].addr);
		int fd= accept(LFD, (struct sockaddr*)&temp_addr, &(FD[i].addr_len));
		int j;
		for (j = 0; j < info.simulated_link_num; j++)
		{
			if (strcmp(FD[j].ip, inet_ntoa(temp_addr.sin_addr)) == 0)
			{
				FD[j].fd = fd;
				memcpy(&FD[j].addr, &temp_addr, sizeof(struct sockaddr_in));
				printf("(device)%d (ip)%s (fd)%d is connected\n", j, inet_ntoa(FD[j].addr.sin_addr), FD[j].fd);
				break;
			}
		}
	}

	/*本机作为客户端 连上比自身索引号小的设备*/
	for (i = MY_INDEX - 1; i >= 0; i--)
	{
		FD[i].fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		memset(&FD[i].addr, 0, sizeof(FD[i].addr));
		FD[i].addr.sin_family = AF_INET;
		FD[i].addr.sin_port = htons(info.communication_port);
		inet_pton(AF_INET, FD[i].ip, (void*)&FD[i].addr.sin_addr);

		printf("is connecting to (device)%d (ip)%s (fd)%d\n", i, FD[i].ip,FD[i].fd);
		while (0 != connect(FD[i].fd, (struct sockaddr*)&FD[i].addr, sizeof(FD[i].addr)))
		{
			perror("link connect");
		}

		printf("connect device %d ip %s succeed\n", i, inet_ntoa(FD[i].addr.sin_addr));
	}

	printf("all link complete\n");

	return 0;
}


void link_complete_boardcast()
{
	int i = 0;
	start_boardcast_t boardcast_msg;

	clock_gettime(CLOCK_MONOTONIC, &boardcast_msg.base_time);
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
