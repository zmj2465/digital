#ifndef _DISPLAY_SEND_THREAD_H_
#define _DISPLAY_SEND_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"

void* display_send_thread(void* arg);

void display_send_thread_init();

#endif