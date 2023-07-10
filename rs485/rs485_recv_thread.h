#ifndef _RS485_RECV_THREAD_H_
#define _RS485_RECV_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"
#include "queue.h"
#include "display_send_thread.h"



void* rs_485_recv_thread(void* arg);



#endif
