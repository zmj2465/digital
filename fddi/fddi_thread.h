#ifndef _FDDI_THREAD_H_
#define _FDDI_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"
#include "display_send_thread.h"

void* fddi_thread(void* arg);

void fddi_thread_init();

void generate_show_msg();

#endif