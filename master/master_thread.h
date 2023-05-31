#ifndef _MASTER_THREAD_H_
#define _MASTER_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"

void* master_thread(void* arg);
int master_proc(void);

#endif