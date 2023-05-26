#ifndef _COMPATIBLE_H_
#define _COMPATIBLE_H_

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <time.h>
#pragma comment(lib, "kernel32.lib")
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#include <time.h>
#include <sys/time.h>

#endif


/*线程创建模板
    void* rs_485_thread(void* arg)
    {
        pthread_detach(pthread_self());
    }
    pthread_t thread_id; // 线程id
    void* arg = NULL; // 线程参数
    int ret = pthread_create(&thread_id, NULL, rs_485_thread, arg);
    if (ret != 0)
    {
        // 创建线程失败，处理错误
        printf("error\n");
    }
*/


#ifdef _WIN32
#define OVERALL_FILE "../../../ipinfo.txt"
#define PRIVATE_FILE "../../../base_info.txt"
#define SIMULATE_FILE "../../../simulation_config.txt"
#else
#define OVERALL_FILE "../ipinfo.txt"
#define PRIVATE_FILE "../base_info.txt"
#define SIMULATE_FILE "../simulation_config.txt"
#endif






void wsa_init();
void udelay(int us);
void set_process_priority();
void set_thread_priority();















#endif