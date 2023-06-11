#ifndef _DATA_SEND_THREAD_H_
#define _DATA_SEND_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"
#include "protocol.h"
#include "schedule_thread.h"

void* data_send_thread(void* arg);
int   data_send_proc(void);
void  generate_packet(uint8_t dst, uint8_t src, uint8_t type, msg_t* msg);

#endif