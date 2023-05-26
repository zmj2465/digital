#include <stdio.h>
#include <pthread.h>
#include "rs485_send_thread.h"
#include "rs485_recv_thread.h"
#include "fddi_thread.h"
#include "schedule_thread.h"
#include "control_recv_thread.h"
#include "display_send_thread.h"
#include "master_thread.h"
#include "link_control_thread.h"
#include "data_recv_thread.h"
#include "data_send_thread.h"
#include "queue.h"

#include "compatible.h"
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include "common.h"

sem_t semaphore;
pthread_mutex_t lock;

static int temp = 0;


int main()
{
    int ret;

    /* */
    wsa_init();

    /* */
    queue_init();

    /* */
    set_process_priority();

    /* */
    load_ip_config();

  

    sem_init(&info.send_semaphore, 0, 0);

    /* */
    ret = pthread_create(&info.rs_485_recv_thread_id, NULL, rs_485_recv_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /* */
    ret = pthread_create(&info.rs_485_send_thread_id, NULL, rs_485_send_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /* */
    ret = pthread_create(&info.fddi_thread_id, NULL, fddi_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /* */
    ret = pthread_create(&info.schedule_thread_id, NULL, schedule_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /* */
    ret = pthread_create(&info.control_recv_thread_id, NULL, control_recv_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /* */
    ret = pthread_create(&info.display_send_thread_id, NULL, display_send_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /* */
    ret = pthread_create(&info.master_thread_id, NULL, master_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /* */
    ret = pthread_create(&info.link_control_thread_id, NULL, link_control_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /* */
    ret = pthread_create(&info.data_send_thread_id, NULL, data_recv_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /* */
    ret = pthread_create(&info.data_recv_thread_id, NULL, data_send_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    //const char* memFileName = "MyMemoryFile";  // 内存文件名

    //HANDLE hFileMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1073741824, memFileName);
    //if (hFileMapping == NULL) {
    //    printf("Failed to create file mapping. Error code: %d\n", GetLastError());
    //    return 1;
    //}

    //// 将文件映射到进程的地址空间
    //LPVOID memPtr = MapViewOfFile(hFileMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    //if (memPtr == NULL) {
    //    printf("Failed to map view of file. Error code: %d\n", GetLastError());
    //    CloseHandle(hFileMapping);
    //    return 1;
    //}



    while (1)
    {
        udelay(200000);
    }
}


void load_self_config()
{

}

void load_ip_config()
{
    FILE* file;
    //char ip[50];
    char line[100];
    // 打开文件
    file = fopen(OVERALL_FILE, "r");
    if (file == NULL) {
        printf("无法打开文件。\n");
        return 1;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        // 去除行尾的换行符
        line[strcspn(line, "\n")] = '\0';
        // 检查是否为"[IP]"行，如果是，则跳过
        if (strcmp(line, "[IP]") == 0) {
            while (fgets(line, sizeof(line), file) != NULL)
            {
                line[strcspn(line, "\n")] = '\0';
                if (strcmp(line, "[END]")==0) break;
                // 存储IP地址到数组
                strncpy(FD[FD_NUM].ip, line, IP_LEN);
                FD_NUM++;
            }
            break;
        }
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        // 去除行尾的换行符
        line[strcspn(line, "\n")] = '\0';
        // 检查是否为"[PORT]"行
        if (strcmp(line, "[PORT]") == 0) {
            // 读取下一行的端口号
            fgets(line, sizeof(line), file);
            line[strcspn(line, "\n")] = '\0';
            sscanf(line, "communication_port %d", &info.communication_port);
            fgets(line, sizeof(line), file);
            sscanf(line, "fddi_port %d", &info.fddi_port);
            fgets(line, sizeof(line), file);
            sscanf(line, "control_port %d", &info.control_port);
            fgets(line, sizeof(line), file);
            sscanf(line, "display_port %d", &info.display_port);
        }
    }

    // 关闭文件
    fclose(file);

    file = fopen(PRIVATE_FILE, "r");
    if (file == NULL) {
        printf("无法打开文件。\n");
        return 1;
    }

    fgets(line, sizeof(line), file);
    sscanf(line, "%s", info.ip);

    fgets(line, sizeof(line), file);
    sscanf(line, "%s", info.fddi_ip);

    fgets(line, sizeof(line), file);
    sscanf(line, "%s", info.control_ip);

    fgets(line, sizeof(line), file);
    sscanf(line, "%s", info.display_ip);

    fclose(file);

    // 打印读取的值
    printf("IP地址: %s\n", FD[0].ip);
    printf("IP地址: %s\n", FD[1].ip);
    printf("IP地址: %s\n", FD[2].ip);
    printf("communication_port: %d %s\n", info.communication_port,info.ip);
    printf("fddi_port: %d %s\n", info.fddi_port, info.fddi_ip);
    printf("control_port: %d %s\n", info.control_port, info.control_ip);
    printf("display_port: %d %s\n", info.display_port, info.display_ip);
}

void load_simulation_config()
{

}