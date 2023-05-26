#include "link_control_thread.h"

void* link_control_thread(void* arg)
{
    pthread_detach(pthread_self());

	configure_init();

	if (MY_INDEX == -1)
	{
		printf("config error\n");
		return -1;
	}

	link_init();

	


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
		printf("wait for %d connect", i);
		FD[i].addr_len = sizeof(FD[i].addr);
		FD[i].fd = accept(LFD, (struct sockaddr*)&(FD[i].addr), &(FD[i].addr_len));
		printf("device %d ip %s is connected\r\n", i, inet_ntoa(FD[i].addr.sin_addr));
	}

	/*本机作为客户端 连上比自身索引号小的设备*/
	for (i = 0; i < MY_INDEX; i++)
	{
		printf("is connecting to %d", i);
		FD[i].fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		memset(&FD[i].addr, 0, sizeof(FD[i].addr));
		FD[i].addr.sin_family = AF_INET;
		FD[i].addr.sin_port = htons(info.communication_port);
		inet_pton(AF_INET, FD[i].ip, (void*)&FD[i].addr.sin_addr);
		while (0 != connect(FD[i].fd, (struct sockaddr*)&FD[i].addr, sizeof(FD[i].addr)));
		printf("connect device %d ip %s succeed\r\n", i, inet_ntoa(FD[i].addr.sin_addr));
	}

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
		}
	}
}


void init_complete_judge()
{


}
