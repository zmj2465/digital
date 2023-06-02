#include "master_thread.h"

/*
���ܣ������߳�
��������
����ֵ����
*/
void* master_thread(void* arg)
{
    pthread_detach(pthread_self());

    while (1)
    {
        /*���ƶ��д���*/
        //dequeue(&info.thread_queue[MASTER_THREAD], msg.data, &msg.len);

        /*���ݶ��д���*/
        master_proc();

        Sleep(3);
        
        /*�·�*/
    }
}

/*
���ܣ������߳����ݶ��е����ݴ���
��������
����ֵ����
*/
int master_proc(void)
{
    msg_t msg;
    dequeue(&info.thread_queue[MASTER_THREAD_DATA], &msg, &msg.len);

    /*�ŵ�����*/

    /*���*/
    msg.len = msg.len - sizeof(head_t) - sizeof(int);

    if (msg.head.dst == info.device_info[MY_INDEX].node_id)
    {
        switch (msg.head.type)
        {
        case SLEF_TEST:

            break;
        case SLEF_TEST_RESULT:

            break;
        case PARAMETER_LOAD:

            break;
        case SHORT_FRAME:

            break;
        case LONG_FRAME:

            break;
        case START_GUN:
            /*����M*/
            if (MY_INDEX == 0)
            {
                if (msg.data[0] == 2)
                {
                    /*ͬ�����潨��ʱ�䣬������״̬����Ϊfsm_wsn*/
                    struct timespec str_m;
                    clock_gettime(CLOCK_MONOTONIC, &str_m);
                    while ((str_m.tv_sec * 1000 + str_m.tv_nsec / 1000000) != (info.str.base_time.tv_nsec * 1000 + info.str.base_time.tv_nsec / 1000000 + info.str.start_time))
                    {
                        clock_gettime(CLOCK_MONOTONIC, &str_m);
                    }
                    /*���㽨��ʱ�䣬��ʼʱ��*/
                    fsm_do(EVENT_WAIT_SIMULATE);

                    
                }
                else
                {
                    printf("M not receive start_gun response\n");
                }
            }
            /*�ӻ�Z*/
            else
            {
                if (msg.data[0] == 1)
                {
                    /*��Ӧ�����ķ���ǹ֡*/
                    msg.data[0] = 2;
                    msg.len = 1;
                    enqueue(&info.thread_queue[DATA_SEND_THREAD], msg.data, msg.len);


                    /*ͬ�����潨��ʱ�䣬������״̬����Ϊfsm_wan*/
                    clock_gettime(CLOCK_MONOTONIC, &info.str.base_time);
                    while ((info.str.base_time.tv_sec * 1000 + info.str.base_time.tv_nsec / 1000000) != (msg.head.sbt.base_time.tv_sec * 1000 + msg.head.sbt.base_time.tv_nsec / 1000000 + msg.head.sbt.start_time))
                    {
                        clock_gettime(CLOCK_MONOTONIC, &info.str.base_time);
                    }
                    fsm_do(EVENT_WAIT_ACCESS);

                }
                else
                {
                    printf("Z not receive start_gun require\n");
                }
            }
            break;
        default:
            break;
        }
    }
    else
    {
        printf("dst uncorrectly, master thread receive failed\n");
    }

    
    return 0;
}

void  master_recv(msg_t* msg)
{
    if (msg->head.dst == info.device_info[MY_INDEX].node_id)
    {
        printf("dst success, receive corretly\n");

    }
    else
    {
        printf("dst failed\n");
    }
}
