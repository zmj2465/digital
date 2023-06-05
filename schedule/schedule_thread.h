#ifndef _SCHEDULE_THREAD_H_
#define _SCHEDULE_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "compatible.h"
#include "common.h"



void* schedule_thread(void* arg);
int schedule_slot(void);
static void timer_handle(int para);
void send_slot(void);
void receive_slot(void);

#endif