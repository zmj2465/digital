#ifndef _DATA_RECV_THREAD_H_
#define _DATA_RECV_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"


void* data_recv_thread(void* arg);

int recv_proc();

#endif