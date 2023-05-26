#ifndef _LINK_CONTROL_THREAD_H_
#define _LINK_CONTROL_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"

#define PORT 7777

void* link_control_thread(void* arg);
void configure_init();




#endif