#include "display_recv_thread.h"
#include "display_send_thread.h"




void* display_recv_thread(void* arg)
{
	pthread_detach(pthread_self());
	show_t msg;
	int len = 0;
	while (1)
	{
		dequeue(&info.thread_queue[DISPLAY_RECV_THREAD], &msg, &len);
		send_to_display(&msg, msg.len);
		Sleep(5);
	}
}






