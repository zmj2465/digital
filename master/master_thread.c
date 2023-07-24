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

    }
}

/*
���ܣ������߳����ݶ��еĴ���
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
                index = inquire_address(msg.head.src);

#ifdef _WIN32
                /*M������֡��ʱ��*/
                timeKillEvent(info.timerId_M[index]);
                info.timerId_M[index] = timeSetEvent(TIMER_DELAY, 0, TimerCallback, index, TIME_ONESHOT);
#endif

                //plog("M recv Z%d data, current slot = %d.%d, seq = %d\n", index, info.current_time_frame, info.current_slot, msg.head.seq);
                break;
            case START_GUN:
                if (msg.data[0] == START_GUN_RES)
                {
                    fsm_do(EVENT_WAIT_SIMULATE);
                }
                else
                {
                    //plog("M not receive start_gun response\n");
                }
                break;
            case SCAN:
                if (msg.data[0] == SCAN_RES)
                {
                    index = inquire_address(msg.head.src);
                    info.device_info.node_num++;
                    info.device_info.node_list = info.device_info.node_list | (1 << index);
                    info.scan_flag_M[index] = 1;
                    //plog("M recv Z%d scan response, list = %d, current slot = %d.%d, seq = %d\n", index, info.device_info.node_list, info.current_time_frame, info.current_slot, msg.head.seq);
                }
                else
                {
                    //plog("M not receive scan response\n");
                }
                break;
            case DISTANCE:
                if (msg.data[0] == DISTANCE_Z)
                {
                    index = inquire_address(msg.head.src);
                    info.distance_flag_M[index] = 1;
                    //plog("M recv Z%d distance frame, current slot = %d.%d, seq = %d\n", index, info.current_time_frame, info.current_slot, msg.head.seq);
                }
                else
                {
                    //plog("M not receive Z distance frame\n");
                }
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
                        info.str.base_time = msg.head.send_time;
                        //info.str.base_t = msg.head.send_t;
                        info.str.start_time = msg.data[1];
                        plog("Z%d base time=%lld, %ld, start_time = %d\n", MY_INDEX, info.str.base_time.tv_sec, info.str.base_time.tv_nsec, info.str.start_time);
                        //plog("Z%d base time=%lld ns, start time = %d s\n", MY_INDEX, info.str.base_t, info.str.start_time);
                        /*��Ӧ�����ķ���ǹ֡*/
                        msg.data[0] = START_GUN_RES;
                        msg.len = 1;
                        generate_packet(info.device_info.node_id[0], info.device_info.node_id[MY_INDEX], START_GUN, &msg);
                        send(FD[0].fd, &msg, sizeof(msg_t), 0);
                        fsm_do(EVENT_WAIT_ACCESS);
                    }
                    else
                    {
                        //plog("Z not receive start_gun require\n");
                    }
                    break;
                case SCAN:
                    if (msg.data[0] == SCAN_REQ)
                    {
#ifdef _WIN32
                        /*�ر�ɨ��ѯ�ʶ�ʱ��*/
                        timeKillEvent(info.timerId);
#endif
                        /*ɨ����Ӧ֡*/
                        info.scan_flag_Z = 1;
                        //plog("Z%d recv M scan request, current slot = %d.%d, seq = %d\n", MY_INDEX, info.current_time_frame, info.current_slot, msg.head.seq);
                    }
                    else if (msg.data[0] == SCAN_CON)
                    {
#ifdef _WIN32
                        /*�ر�ɨ��ظ���ʱ��*/
                        timeKillEvent(info.timerId);
#endif
                        info.time_frame_flag_z = msg.data[1];
                        //plog("Z%d recv M scan confirm, current slot = %d.%d, seq = %d\n", MY_INDEX, info.current_time_frame, info.current_slot, msg.head.seq);
                        fsm_do(EVENT_WAN_SUCC);
                    }
                    else
                    {
                        //plog("Z not receive scan\n");
                    }
                    break;
                case LONG_FRAME:
#ifdef _WIN32
                    /*Z������֡��ʱ��*/
                    timeKillEvent(info.timerId_Z);
                    info.timerId_Z = timeSetEvent(TIMER_DELAY, 0, TimerCallback, Z_DATA_TIMER, TIME_ONESHOT);
#endif
                    //plog("Z%d recv M data, current slot = %d.%d, seq = %d\n", MY_INDEX, info.current_time_frame, info.current_slot, msg.head.seq);       
                    break;
                default:
                    break;
            }

        }
    }
    else
    {
        //plog("dst uncorrectly, master thread receive failed\n");
    }

    return 0;
}

/*
���ܣ������߳̿��ƶ��еĴ���
��������
����ֵ����
*/
int master_control_proc(void)
{

    return 0;
}