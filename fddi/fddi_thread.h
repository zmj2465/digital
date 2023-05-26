#ifndef _FDDI_THREAD_H_
#define _FDDI_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"

void* fddi_thread(void* arg);

void fddi_thread_init();


#endif