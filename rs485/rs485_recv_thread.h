#ifndef _RS485_RECV_THREAD_H_
#define _RS485_RECV_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"
#include "queue.h"


#define SP_LEN sizeof(sp_info_t)

#pragma pack(1)
typedef struct {
    uint32_t seq;
    uint8_t  type;
    uint8_t  mz[500];
    uint8_t  zz[60];
}sp_info_t;

typedef struct {
    uint32_t start;
    uint8_t  dst;
    uint8_t  src;
    uint8_t  type;
    uint8_t  sp[SP_LEN];
    uint16_t crc;
    uint32_t end;
}rs485_info_t;
#pragma pack()


void* rs_485_recv_thread(void* arg);



#endif
