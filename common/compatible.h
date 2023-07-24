#ifndef _COMPATIBLE_H_
#define _COMPATIBLE_H_

#define DESKTOP			1 //置1系统时钟，置0 PTP时钟

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <time.h>
#include <stdint.h>
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





uint64_t get_time_swtich(void);
void wsa_init();
void udelay(int us);
void set_process_priority();
void set_thread_priority();
int setNonBlocking(int sockfd);

#endif