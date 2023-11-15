#ifndef _DISPLAY_RECV_THREAD_H_
#define _DISPLAY_RECV_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include <dirent.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"
#include "display_thread.h"

void* display_recv_thread(void* arg);


#endif
