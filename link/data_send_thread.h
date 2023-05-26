#ifndef _DATA_SEND_THREAD_H_
#define _DATA_SEND_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"

void* data_send_thread(void* arg);

#endif