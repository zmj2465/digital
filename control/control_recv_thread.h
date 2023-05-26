#ifndef _CONTROL_RECV_THREAD_H_
#define _CONTROL_RECV_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"



void* control_recv_thread(void* arg);

void control_recv_thread_init();

#endif