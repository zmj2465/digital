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
        
        
        /*���ݶ��д���*/
        master_data_proc();

        Sleep(3);

        /*�·�*/
    }
}

/*
���ܣ������߳����ݶ��е����ݴ���
��������
����ֵ����
*/
int master_data_proc(void)
{
    int index;
    msg_t msg;
    dequeue(&info.thread_queue[MASTER_THREAD_DATA], &msg, &msg.len);

    /*���*/
    msg.len = msg.len - sizeof(head_t) - sizeof(int);

    if (msg.head.dst == info.device_info.node_id[MY_INDEX])
    {
        /*����*/
        if (MY_INDEX == 0)
        {
            switch (msg.head.type)
            {
            case SHORT_FRAME:
                break;
            case LONG_FRAME:
                break;
            case START_GUN:
                if (msg.data[0] == START_GUN_RES)
                {
                    fsm_do(EVENT_WAIT_SIMULATE);
                }
                else
                {
                    printf("M not receive start_gun response\n");
                }
                break;
            case SCAN:
                if (msg.data[0] == SCAN_RES)
                {
                    index = inquire_slot(info.current_slot);
                    info.device_info.node_num++;
                    info.device_info.node_list = info.device_info.node_list | (1 << index);
                    info.scan_flag[index] = 1;
                    printf("list =  %d\n", info.device_info.node_list);
                    //if (info.device_info.node_num == FD_NUM)
                    //{
                    //    fsm_do(EVENT_WSN_SUCC);
                    //}
                    ///*ɨ��ظ�֡*/
                    //msg.data[0] = 3;
                    //msg.len = 1;
                    //enqueue(&info.thread_queue[DATA_SEND_THREAD_SCAN_CON], msg.data, msg.len);
                }
                else
                {
                    printf("M not receive scan response\n");
                }
                break;
            default:
                break;
            }
        }
        /*�ӻ�*/
        else
        {
            switch (msg.head.type)
            {
                case START_GUN:
                    if (msg.data[0] == START_GUN_REQ)
                    {
                        info.str.start_time = msg.data[1];
                        printf("Z base time=%lld, %ld, start_time = %d\n", msg.head.send_time.tv_sec, msg.head.send_time.tv_nsec, info.str.start_time);
                        /*��Ӧ�����ķ���ǹ֡*/
                        msg.data[0] = START_GUN_RES;
                        msg.len = 1;
                        generate_packet(info.device_info.node_id[0], info.device_info.node_id[MY_INDEX], START_GUN, &msg);
                        send(FD[0].fd, &msg, msg.len, 0);

                        /*ͬ�����潨��ʱ�䣬������״̬����Ϊfsm_wan*/
                        clock_gettime(CLOCK_REALTIME, &info.str.base_time);
                        while ((info.str.base_time.tv_sec * 1000000000 + info.str.base_time.tv_nsec) < (msg.head.send_time.tv_sec * 1000000000 + msg.head.send_time.tv_nsec + info.str.start_time * 1000000000))
                        {
                            clock_gettime(CLOCK_REALTIME, &info.str.base_time);
                        }
                        fsm_do(EVENT_WAIT_ACCESS);
                    }
                    else
                    {
                        printf("Z not receive start_gun require\n");
                    }
                    break;
                case SCAN:
                    if (msg.data[0] == SCAN_REQ)
                    {
                        /*�ر�ɨ��ѯ�ʶ�ʱ��*/
                        timeKillEvent(info.timerId);
                        /*ɨ����Ӧ֡*/
                        msg.data[0] = SCAN_RES;
                        msg.len = 1;
                        enqueue(&info.thread_queue[DATA_SEND_THREAD], msg.data, msg.len);
                    }
                    else if (msg.data[0] == SCAN_CON)
                    {
                        /*�ر�ɨ��ظ���ʱ��*/
                        timeKillEvent(info.timerId);
                        fsm_do(EVENT_WAN_SUCC);
                    }
                    else
                    {
                        printf("Z not receive scan require\n");
                    }
                    break;
                default:
                    break;
            }

        }
    }
    else
    {
        printf("dst uncorrectly, master thread receive failed\n");
    }

    return 0;
}

