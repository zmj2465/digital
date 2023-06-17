#ifndef _MASTER_THREAD_H_
#define _MASTER_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"
#include "protocol.h"
#include "data_send_thread.h"

void* master_thread(void* arg);
int	  master_data_proc(void);
int	  master_control_proc(void);
#endif