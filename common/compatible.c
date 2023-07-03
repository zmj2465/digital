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

#define CLOCK_REALTIME 0

void udelay(int us)
{

    uint64_t temp;
    struct timespec t_start, t_end;
    clock_gettime(CLOCK_REALTIME, &t_start);

    while (1)
    {

        clock_gettime(CLOCK_REALTIME, &t_end);
        temp = (t_end.tv_sec - t_start.tv_sec) * 1000000000 + (t_end.tv_nsec - t_start.tv_nsec);
        if (temp >= us * 1000) break;

    }
}
