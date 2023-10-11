#ifndef _DISPLAY_THREAD_H_
#define _DISPLAY_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include <dirent.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"
#include "display_send_thread.h"


#define MAX_SEND_LEN 1024

void* display_thread(void* arg);

void create_table(show_t* msg);

#endif