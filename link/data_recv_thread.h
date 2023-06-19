#ifndef _DATA_RECV_THREAD_H_
#define _DATA_RECV_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"
#include "physical_simulation.h"
#include "protocol.h"
#include "schedule_thread.h"

void* data_recv_thread(void* arg);
int   data_recv_proc(void);
int	  antenna_match(char* data, msg_t* msg, int role);

#endif