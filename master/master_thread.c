#include "master_thread.h"

/*
���ܣ������߳�
��������
����ֵ����
*/
void* master_thread(void* arg)
{
    pthread_detach(pthread_self());

    msg_t msg;

    while (1)
    {
        /*������Ϣ*/
        dequeue(&info.thread_queue[MASTER_THREAD], msg.data, &msg.len);
        /*���ݶ��д���*/
        master_proc();
        sleep(3);
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
        if (MY_INDEX == 0)//����
        {
        }
        else//�ӻ�
        {
            /*�����ϵͳʱ�䣬������״̬����Ϊfsm_wan*/
            /*send*/
        }
        break;
    default:
        break;
    }
    return 0;
}