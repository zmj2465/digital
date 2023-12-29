#include <stdio.h>
#include <pthread.h>

#include "rm_thread.h"
#include "rs485_send_thread.h"
#include "rs485_recv_thread.h"
#include "fddi_thread.h"
#include "schedule_thread.h"
#include "control_recv_thread.h"
#include "display_send_thread.h"
#include "display_recv_thread.h"
#include "display_store_thread.h"
#include "display_thread.h"
#include "master_thread.h"
#include "link_control_thread.h"
#include "data_recv_thread.h"
#include "data_send_thread.h"
#include "file_manage.h"
#include "log.h"
#include "queue.h"

#include "compatible.h"
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include "common.h"
#include "protocol.h"
#include "main.h"
#include "sql.h"
#include "mytime.h"

#include "comcal_dll.h"

#include "windows.h"

sem_t semaphore;
pthread_mutex_t lock;


bool ctrlhandler(DWORD fdwctrltype)
{

}





int main()
{
    int a=0;
    float b=0;
    float c=0;
    calculate_ante_angle_coord_m(6305.516113, 490.071899
        , 1772.316162
        , -0.405952
        , -0.606029
        , 0.492796
        , 0.474429
        , 0
        , 6302.225098
        , 486.320496
        , 1774.466064
        , &a, &b, &c);
    //my_add(3, 5);
    printf("%d %f %f\n", a, b, c);
    calculate_ante_angle_coord_m(6305.120117, 490.386810
        , 1774.043945
        , -0.405952
        , -0.606029
        , 0.492796
        , 0.474429
        , 0
        , 6302.225098
        , 486.320496
        , 1774.466064
        , &a, &b, &c);
    //my_add(3, 5);
    printf("%d %f %f\n", a, b, c);
    //printf("%d\n", my_add(3, 5));
    //float yaw, roll, pitch;
    //Quaternion ttt;
    //ttt.q0 = 0.9568315;
    //ttt.q1 = 0.03177429;
    //ttt.q2 = -0.2492538;
    //ttt.q3 = -0.1460697;
    //show_t msg;
    //quaternionToEulerAngles(
    //    ttt,
    //    &msg.display_info.roll[0],
    //    &msg.display_info.pitch[0],
    //    &msg.display_info.yaw[0]
    //);
    //while (1);
    //system("C:\\Users\\MRGT\\Desktop\\start.bat");
    //info.chain_flag_m = 1;
    
    /*ptp时钟初始化*/
    time_init();

    /*链接库初始化*/
    wsa_init();

    /*消息队列初始化*/
    queue_init();

    /*设置进程优先级*/
    set_process_priority();

    /*信息配置*/
    config_init();

    log_init();


    /*文件系统初始化*/
    file_init();

    /*线程初始化*/
    thread_init();



    /*设置初始状态*/
    fsm_do(EVENT_INIT);

    while (1)
    {
        sleep(200000);
    }
}


void thread_init()
{
    sem_init(&info.thread_create_semaphore, 0, 0);

    int i;
    int ret = 0;
    /*线程初始化*/
    pthread_t temp;

    /**/
    ret = pthread_create(&info.log_thread_id, NULL, log_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }


    ret = pthread_create(&temp, NULL, rm_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

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
    ret = pthread_create(&info.display_recv_thread_id, NULL, display_recv_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.display_store_thread_id, NULL, display_store_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.display_thread_id, NULL, display_thread, NULL);
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
    if (MY_INDEX == 0)
    {
        printf("***************************舱通信终端数字样机 %s\n", VERSION);
    }
    else
    {
        printf("***************************器通信终端数字样机 %s\n", VERSION);
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
}

void config_init()
{
    int ret = 0;
    load_config(INFO_SET_FILE);
    ret = load_config(INFO_SET_DESK_FILE);
    if (ret == -1) printf("folder config\n");
    else printf("desktop config\n");
    online_state[MY_INDEX] = 1;
    if (MY_INDEX == 0)
    {
        MY_ROLE = 0;
    }
    else
    {
        MY_ROLE = 1;
    }
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
    MY_ID = MY_INDEX;
}



