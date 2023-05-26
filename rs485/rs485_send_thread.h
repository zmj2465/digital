#ifndef _RS485_SEND_THREAD_H_
#define _RS485_SEND_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"



void* rs_485_send_thread(void* arg);



#endif