#ifndef _RS485_RECV_THREAD_H_
#define _RS485_RECV_THREAD_H_

#include <pthread.h>
#include <stdio.h>
#include "compatible.h"
#include <stdlib.h>
#include "common.h"
#include "queue.h"


enum SystemState {
    POWER_ON = 0,
    SELF_TEST = 1,
    REFUELING = 2,
    PRE_SEPARATION = 3,
    SEPARATION = 4,
    ESTABLISHING_LINK = 5,
    BREAKING_LINK = 6,
    RECONNECTION = 7,
    SP = 0x60
};

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
    union {
        uint8_t  sp[SP_LEN];
        uint16_t crc;
        uint32_t end;
    };
    uint16_t crc_;
    uint32_t end_;
}rs485_info_t;
#pragma pack()






void* rs_485_recv_thread(void* arg);






#endif
