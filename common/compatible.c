#include "compatible.h"
#include <stdint.h>
#include <stdio.h>
#include <sched.h>
#include "pthread.h"


void wsa_init()
{
    #ifdef _WIN32
    int ret = 0;
    WSADATA wsaData;
    ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (ret != 0)
    {
        printf("WSAStartup failed: %d\n", ret);
        return ;
    }
    #endif
}



void set_process_priority()
{
#ifdef _WIN32
    HANDLE hProcess = GetCurrentProcess();

    int priority = GetPriorityClass(hProcess);
    printf("Old Process priority: 0x%x\n", priority);

    if (!SetPriorityClass(hProcess, REALTIME_PRIORITY_CLASS))
    {
        printf("Failed to set process priority.\n");
        return ;
    }
    priority = GetPriorityClass(hProcess);
    printf("New Process priority: 0x%x\n", priority);
    CloseHandle(hProcess);
#else
    int priority = getpriority(PRIO_PROCESS, 0);
    printf("Old Process priority: %d\n", priority);

    int pid = getpid();
    struct sched_param param;
    param.sched_priority = 99;
    if (sched_setscheduler(pid, SCHED_FIFO, &param) == -1) {
        perror("Failed to set process priority");
        return ;
    }

    priority = getpriority(PRIO_PROCESS, 0);
    printf("New Process priority: %d\n", priority);
#endif
}

void set_thread_priority()
{
#ifdef _WIN32
    HANDLE hThread = GetCurrentThread();

    int priority = GetThreadPriority(hThread);
    printf("Old Thread priority: %d\n", priority);

    if(!SetThreadPriority(hThread, THREAD_PRIORITY_TIME_CRITICAL))
    {
        printf("Failed to set thread priority \n");
    }
    priority = GetThreadPriority(hThread);
    printf("New Thread priority: %d\n", priority);
#else
    pthread_t thread = pthread_self();
    struct sched_param param;

    // 获取线程当前优先级
    int priority = sched_get_priority_max(SCHED_FIFO);
    printf("Old Thread priority: %d\n", priority);

    // 获取当前调度策略和参数
    int policy;
    pthread_getschedparam(thread, &policy, &param);

    // 将线程设置为实时优先级
    param.sched_priority = sched_get_priority_max(SCHED_FIFO);
    if(!pthread_setschedparam(thread, SCHED_FIFO, &param))
    {
        printf("Failed to set thread priority.\n");
        return ;
    }
    
    priority = sched_get_priority_max(SCHED_FIFO);
    printf("New Thread priority: %d\n", priority);
#endif
}



void udelay(int us)
{
    uint64_t temp;
    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    while (1)
    {
        clock_gettime(CLOCK_MONOTONIC, &t_end);
        temp = (t_end.tv_sec - t_start.tv_sec) * 1000000000 + (t_end.tv_nsec - t_start.tv_nsec);
        if (temp >= us * 1000)
        {
            //tosche("temp = %lldus\n", temp/1000);
            break;
        }
    }
}


uint64_t get_time_swtich(void)
{
#if(DESKTOP == 1)
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec * 1000 * 1000 * 1000 + time.tv_nsec;
#else
    uint64_t time;
    time = my_get_time();
    return time;
#endif
}

int setNonBlocking(int sockfd) {
#ifdef _WIN32
    u_long mode = 1; // 1 表示非阻塞模式，0 表示阻塞模式
    if (ioctlsocket(sockfd, FIONBIO, &mode) != 0) {
        printf("set socket fail\n");
        return -1; // 设置失败
    }
#else
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        return -1; // 设置失败
    }

    flags |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, flags) == -1) {
        return -1; // 设置失败
    }
#endif

    return 0; // 设置成功
}