#ifndef _DISPLAY_STORE_THREAD_H_
#define _DISPLAY_STORE_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include <dirent.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"
#include "display_send_thread.h"


void* display_store_thread(void* arg);

void data_store();
void send_display_msg();
void create_msg(show_t* msg);
void data_show(show_t* msg);

#endif