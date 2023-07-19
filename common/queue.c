#include "queue.h"


//
//void init_msg_queue(msg_queue_t* queue, int max_size) {
//	queue->head = NULL;
//	queue->tail = NULL;
//	queue->size = 0;
//	queue->max_size = max_size;
//	pthread_mutex_init(&queue->lock, NULL);
//	pthread_cond_init(&queue->cond, NULL);
//}
//
//void enqueue(msg_queue_t* queue, void* data, int len)
//{
//	msg_node_t* node = (msg_node_t*)malloc(sizeof(msg_node_t));
//	node->data = malloc(len);
//	memcpy(node->data, data, len);
//	node->len = len;
//	node->next = NULL;
//
//	pthread_mutex_lock(&queue->lock);
//	while (queue->max_size > 0 && queue->size >= queue->max_size) {
//		pthread_cond_wait(&queue->cond, &queue->lock);
//	}
//
//	if (queue->tail == NULL) {
//		queue->head = node;
//		queue->tail = node;
//	}
//	else {
//		queue->tail->next = node;
//		queue->tail = node;
//	}
//
//	queue->size++;
//
//	pthread_cond_signal(&queue->cond);
//	pthread_mutex_unlock(&queue->lock);
//}
//
//void dequeue(msg_queue_t* queue, void** data, int* len)
//{
//	pthread_mutex_lock(&queue->lock);
//
//	while (queue->head == NULL) {
//		pthread_cond_wait(&queue->cond, &queue->lock);
//	}
//
//	msg_node_t* node = queue->head;
//	queue->head = node->next;
//	if (queue->head == NULL) {
//		queue->tail = NULL;
//	}
//	queue->size--;
//
//	memcpy(data, node->data, node->len);
//	*len = node->len;
//	free(node->data);
//	free(node);
//
//	pthread_cond_signal(&queue->cond);
//	pthread_mutex_unlock(&queue->lock);
//}
//
//void destroy_msg_queue(msg_queue_t* queue, int max_size) {
//	pthread_mutex_destroy(&queue->lock);
//	pthread_cond_destroy(&queue->cond);
//	msg_node_t* cur = queue->head;
//	while (cur != NULL) {
//		msg_node_t* next = cur->next;
//		free(cur->data);
//		free(cur);
//		cur = next;
//	}
//}


void init_msg_queue(msg_queue_t* queue, int max_size) {
	queue->head = 0;
	queue->tail = 0;
	queue->max_size = max_size;
	queue->node = (msg_node_t*)malloc(max_size * sizeof(msg_node_t));
	//queue->size = 0;
	pthread_mutex_init(&queue->lock, NULL);
	pthread_cond_init(&queue->cond, NULL);
}


void enqueue(msg_queue_t* queue, void* data, int len)
{
	pthread_mutex_lock(&queue->lock);

	while ((queue->tail + 1) % (queue->max_size) == queue->head)
	{
		pthread_cond_wait(&queue->cond, &queue->lock);
		printf("queue is full\n");
	}

	memcpy(queue->node[queue->tail].data, data, len);
	queue->node[queue->tail].len = len;

	queue->tail = (queue->tail + 1) % queue->max_size;

	pthread_cond_signal(&queue->cond);
	pthread_mutex_unlock(&queue->lock);
}

void dequeue(msg_queue_t* queue, void** data, int* len)
{
	pthread_mutex_lock(&queue->lock);

	while (queue->head == queue->tail)
	{
		pthread_cond_wait(&queue->cond, &queue->lock);
	}

	memcpy(data, queue->node[queue->head].data, queue->node[queue->head].len);
	*len = queue->node[queue->head].len;

	queue->head = (queue->head + 1) % queue->max_size;

	pthread_cond_signal(&queue->cond);
	pthread_mutex_unlock(&queue->lock);
}


int enqueue_no_block(msg_queue_t* queue, void* data, int len)
{
	pthread_mutex_lock(&queue->lock);
	if ((queue->tail + 1) % (queue->max_size) == queue->head)
	{
		printf("queue is full\n", __FUNCTION__);
		pthread_mutex_unlock(&queue->lock);
		return -1;
	}

	memcpy(queue->node[queue->tail].data, data, len);
	queue->node[queue->tail].len = len;

	queue->tail = (queue->tail + 1) % queue->max_size;

	pthread_mutex_unlock(&queue->lock);
}

int dequeue_no_block(msg_queue_t* queue, void** data, int* len)
{
	pthread_mutex_lock(&queue->lock);

	if (queue->head == queue->tail)
	{
		printf("queue is empty\n");
		pthread_mutex_unlock(&queue->lock);
		return -1;
	}

	memcpy(data, queue->node[queue->head].data, queue->node[queue->head].len);
	*len = queue->node[queue->head].len;

	queue->head = (queue->head + 1) % queue->max_size;

	pthread_mutex_unlock(&queue->lock);
}



void destroy_msg_queue(msg_queue_t* queue, int max_size) {
	pthread_mutex_destroy(&queue->lock);
	pthread_cond_destroy(&queue->cond);
	free(queue->node);
}