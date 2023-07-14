#ifndef _COMPATIBLE_H_
#define _COMPATIBLE_H_

#include <stdint.h>

#define DESKTOP 1

#if(DESKTOP==1)
#define my_get_time() get_time_()
#else

#endif


#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <time.h>
#pragma comment(lib, "kernel32.lib")
//#pragma comment(lib, "pcieapi.lib")
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





#ifdef _WIN32
#define OVERALL_FILE "../../../info set/ipinfo.txt"
#define PRIVATE_FILE "../../../info set/base_info.txt"
#define SIMULATE_FILE "../../../info set/simulation_config.txt"
#define OUTPUT_FILE_NAME "../../../aaa.txt"
#define INFO_SET_FILE "../../../info set/info_set.ini"
#define INFO_SET_DESK_FILE "C:\\Users\\xykc\\Desktop\\info_set.ini" 

#define FOLDER_NAME_LEN 100
#define TOTAL_FOLDER "C:\\Digital prototype"
#define LOG_FOLDER   "C:\\Digital prototype\\log"
#define DATA_FOLDER  "C:\\Digital prototype\\data"
#define SCHE_FOLDER   "C:\\Digital prototype\\sche"

#define END_FLAG "\n"
#define DIR_SEPARATOR "\\"
#else
#define OVERALL_FILE "../info set/ipinfo.txt"
#define PRIVATE_FILE "../info set/base_info.txt"
#define SIMULATE_FILE "../info set/simulation_config.txt"
#define INFO_SET_FILE "../info set/info_set.ini"
#define END_FLAG "\r"
#define DIR_SEPARATOR "/"
#endif






void wsa_init();
void udelay(int us);
void set_process_priority();
void set_thread_priority();
int setNonBlocking(int sockfd);
uint64_t get_time_();








//char a[]="aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
//uint64_t c, b;
//while (1)
//{
//    c = my_get_time();
//    tolog("aaaaa %d a %d aaaaaaaaaaaaaaaaaa %f aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n", 100, 100, 2.2);
//    //printf("aaaaa %d a %d aaaaaaaaaaaaaaaaaa %f aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n", 100, 100, 2.2);
//    b = my_get_time();
//    tosche("%lld\n", b - c);
//    Sleep(1000);
//}





#endif