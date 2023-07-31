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
#include "file_manage.h"
#include "queue.h"

#include "compatible.h"
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include "common.h"
#include "protocol.h"
#include "main.h"

#include "mytime.h"

sem_t semaphore;
pthread_mutex_t lock;


int main()
{

    time_init();

    int ret = 0;

    /*链接库初始化*/
    wsa_init();

    /*消息队列初始化*/
    queue_init();

    /*设置进程优先级*/
    set_process_priority();

    ///*ip信息配置*/
    load_config(INFO_SET_FILE);
    ret=load_config(INFO_SET_DESK_FILE);
    if (ret == -1) printf("folder config\n");
    else printf("desktop config\n");

    file_init();


    //FD_ZERO(&RSET);
    ////FD_SET(200, &RSET);
    //int num = select(1, &RSET, NULL, NULL, NULL);
    //printf("num %d\n",num);
    //while (1);



    /*信号量初始化*/   
    pthread_spin_init(&start_spin,PTHREAD_PROCESS_PRIVATE);
    sem_init(&info.send_semaphore, 0, 0);
    sem_init(&info.thread_create_semaphore, 0, 0);


    /*线程初始化*/
    ret = pthread_create(&info.rs_485_recv_thread_id, NULL, rs_485_recv_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.rs_485_send_thread_id, NULL, rs_485_send_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.fddi_thread_id, NULL, fddi_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.schedule_thread_id, NULL, schedule_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.control_recv_thread_id, NULL, control_recv_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }


    /**/
    ret = pthread_create(&info.display_send_thread_id, NULL, display_send_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.master_thread_id, NULL, master_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.link_control_thread_id, NULL, link_control_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }


    sem_wait(&info.thread_create_semaphore);

    /**/
    ret = pthread_create(&info.data_send_thread_id, NULL, data_recv_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.data_recv_thread_id, NULL, data_send_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /*设置初始状态*/
    fsm_do(EVENT_INIT);

    while (1)
    {
        sleep(200000);
    }
}


void load_self_config()
{

}


void load_simulation_config()
{

}

void data_store_init()
{
    //// 打开文件
    //HANDLE hFile = CreateFile(OUTPUT_FILE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    //if (hFile == INVALID_HANDLE_VALUE) {
    //    printf("Failed to open file\n");
    //    return 1;
    //}

    //// 设置文件大小
    //SetFilePointer(hFile, STORE_SIZE, NULL, FILE_BEGIN);
    //SetEndOfFile(hFile);

    //// 创建内存映射
    //HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, STORE_SIZE, NULL);
    //if (hMapping == NULL) {
    //    printf("Failed to create file mapping\n");
    //    CloseHandle(hFile);
    //    return 1;
    //}

    //// 映射到进程的内存空间
    //info.lpSharedMem = MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, STORE_SIZE);
    //if (info.lpSharedMem == NULL) {
    //    printf("Failed to map view of file\n");
    //    CloseHandle(hMapping);
    //    CloseHandle(hFile);
    //    return 1;
    //}
}


/*
    * 函数名：         GetIniKeyString
    * 入口参数：        title
    *                      配置文件中一组数据的标识
    *                  key
    *                      这组数据中要读出的值的标识
    *                  filename
    *                      要读取的文件路径
    * 返回值：         找到需要查的值则返回正确结果 0
    *                  否则返回-1
    */
int GetIniKeyString(char* title, char* key, char* filename, char* buf)
{
    FILE* fp;
    int  flag = 0;
    char sTitle[64], * wTmp;
    char sLine[1024];
    sprintf(sTitle, "[%s]", title);

    if (NULL == (fp = fopen(filename, "r"))) {
        perror("fopen");
        return -1;
    }

    while (NULL != fgets(sLine, 1024, fp)) {
        // 这是注释行  
        if (0 == strncmp("//", sLine, 2)) continue;
        if ('#' == sLine[0])              continue;
        wTmp = strchr(sLine, '=');
        if ((NULL != wTmp) && (1 == flag)) {
            if (0 == strncmp(key, sLine, strlen(key))) { // 长度依文件读取的为准
#ifdef _WIN32
                sLine[strlen(sLine) - 1] = '\0';
#else
                sLine[strlen(sLine) - 2] = '\0';
#endif
                fclose(fp);
                while (*(wTmp + 1) == ' ') {
                    wTmp++;
                }
                strcpy(buf, wTmp + 1);
                return 0;
            }
        }
        else {
            if (0 == strncmp(sTitle, sLine, strlen(sTitle))) { // 长度依文件读取的为准  
                flag = 1; // 找到标题位置  
            }
        }
    }
    fclose(fp);
    return -1;
}

int load_config(char* filename)
{

    FILE* fp;
    if (NULL == (fp = fopen(filename, "r"))) {
        return -1;
    }

    char name[20];
    char num[20];
    char key[20];
    char buff[100][20];
    int ret = 0;
    int i = 0;
    int j;
    MY_INDEX = -1;

    //仿真节点数量
    ret = GetIniKeyString("NUM", "num", filename, num);
    //printf("%d %s\n", ret, num);
    FD_NUM = atoi(num);

    //tolog("SIM_NUM:%d\n", FD_NUM);
    printf("SIM_NUM:%d\n", FD_NUM);

    //本节点信息
    ret = GetIniKeyString("MY_NAME", "name", filename, name);
    printf("My Real Name:%s\n", name);

    ret = GetIniKeyString(name, "ip", filename, (char*)&buff[++i]);
    strcpy(info.ip, buff[i]);

    ret = GetIniKeyString(name, "port", filename, (char*)&buff[++i]);
    info.port = atoi(buff[i]);

    ret = GetIniKeyString(name, "fddi_ip", filename, (char*)&buff[++i]);
    strcpy(info.fddi_ip, buff[i]);

    ret = GetIniKeyString(name, "fddi_port", filename, (char*)&buff[++i]);
    info.fddi_port = atoi(buff[i]);

    ret = GetIniKeyString(name, "control_ip", filename, (char*)&buff[++i]);
    strcpy(info.control_ip, buff[i]);

    ret = GetIniKeyString(name, "control_port", filename, (char*)&buff[++i]);
    info.control_port = atoi(buff[i]);

    ret = GetIniKeyString(name, "display_ip", filename, (char*)&buff[++i]);
    strcpy(info.display_ip, buff[i]);

    ret = GetIniKeyString(name, "display_port", filename, (char*)&buff[++i]);
    info.display_port = atoi(buff[i]);

    printf("\n***[My Info]***\n");
    printf("ip : %s\n", info.ip);
    printf("port : %d\n", info.port);

    printf("fddi_ip : %s\n", info.fddi_ip);
    printf("fddi_port : %d\n", info.fddi_port);

    printf("control_ip : %s\n", info.control_ip);
    printf("control_port : %d\n", info.control_port);

    printf("display_ip : %s\n", info.display_ip);
    printf("display_port : %d\n", info.display_port);

    printf("\n***[Overall Info]***\n", info.display_port);
    //所有节点信息
    for (j = 0; j < atoi(num); j++)
    {
        char value[20];
        int ip_index;
        int port_index;
        sprintf(key, "L%d", j);
        ret = GetIniKeyString("NUM", key, filename, value);
        //printf("%d %s\n", ret, value);

        ret = GetIniKeyString(value, "ip", filename, (char*)&buff[++i]);
        //printf("%d %s\n", ret, buff[i]);
        ip_index = i;

        ret = GetIniKeyString(value, "port", filename, (char*)&buff[++i]);
        //printf("%d %s\n", ret, buff[i]);
        port_index = i;

        if (strcmp(value, name) == 0)
        {
            MY_INDEX = j;
        }

        strcpy(FD[j].ip, buff[ip_index]);
        FD[j].port = atoi(buff[port_index]);
        printf("[%s] [%s] ", key, value);
        if (MY_INDEX == j) printf(" <----\n");
        else printf("\n");
        printf("ip : %s\n", FD[j].ip);
        printf("port : %d\n", FD[j].port);
    }
    printf("\n");
}



