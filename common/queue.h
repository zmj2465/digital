#ifndef _QUEUE__H_
#define _QUEUE__H_

#include <pthread.h>
#include <stdlib.h>
#include <string.h>


#include "compatible.h"

#define MAX_DATA_LEN_QUEUE 1024

//typedef struct _msg_node_t {
//	char data[MAX_DATA_LEN];
//	int len;
//	struct _msg_node_t* next;
//}msg_node_t;
//
//typedef struct _msg_queue_t {
//	msg_node_t* head;
//	msg_node_t* tail;
//	int max_size;
//	int size;
//	pthread_mutex_t lock;
//	pthread_cond_t cond;
//}msg_queue_t;
//

typedef struct _msg_node_t {
	char data[MAX_DATA_LEN_QUEUE];
	int len;
}msg_node_t;

typedef struct _msg_queue_t {
	msg_node_t* node;
	int max_size;
	//int size;
	int head;
	int tail;
	pthread_mutex_t lock;
	pthread_cond_t cond;
}msg_queue_t;

void init_msg_queue(msg_queue_t* queue, int max_size);
void enqueue(msg_queue_t* queue, void* data, int len);
void dequeue(msg_queue_t* queue, void** data, int* len);


int enqueue_no_block(msg_queue_t* queue, void* data, int len);
int dequeue_no_block(msg_queue_t* queue, void** data, int* len);

#endif