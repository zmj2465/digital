#include "schedule_thread.h"

#define ANTENNA_NUM   6   /*天线数*/
#define SLOT_NUM	  63  /*时隙数*/


static int antenna_table[ANTENNA_NUM] = { 0, 0, 0, 0, 0, 0 };      //0接收1发送

static int slot_table[SLOT_NUM] = 
{800, 800, 800, 800, 800,        //0-4       M1_SEND_Z1
 800, 800, 800, 800, 800,        //5-9       M1_SEND_Z2
 800, 800, 800, 800,             //10-13     M1_SEND_Z3
 800, 800, 800, 800,             //14-17     M1_SEND_Z4   
 800, 800, 800, 800, 800, 800,   //18-23     M1_SEND_M2
 800, 800, 800, 800, 800,        //24-28     M1_SNED_Z5
 4000,                           //29        PROTECT1  
 2600, 2600, 2600, 2600, 2600,   //30-34     Z1_SEND_M1
 2600, 2600, 2600, 2600, 2600,   //35-39     Z2_SEND_M1
 2600, 2600, 2600, 2600,         //40-43     Z3_SEND_M1
 2600, 2600, 2600, 2600,         //44-47     Z4_SEND_M1
 2600, 2600, 2600, 2600, 2600,   //48-52     Z5_SEND_M1
 800, 800, 800, 800, 800, 800,   //53-58     M2_SEND_M1
 2600,                           //59        Z_DISTANCE_M1
 4000,                           //60        PROTECT2
 800,                            //61        M1_DISTANCE_Z
 800                             //62        RESERVE
 };


void* schedule_thread(void* arg)
{
    pthread_detach(pthread_self());
    set_thread_priority();

    while (1)
    {
        if (fsm_status == FSM_WSN || fsm_status == FSM_WAN || fsm_status == FSM_ON)
        {
            schedule_slot();
        }
    }
}

/*
功能：时隙调度初始化
参数：无
返回值：0表示成功
*/
void schedule_slot_init(void)
{
    info.current_slot    = 0;
    info.current_antenna = 0;
    memset(info.scan_flag, 0, sizeof(info.scan_flag));
    memset(antenna_table, 0, sizeof(antenna_table));
}


/*
功能：时隙调度
参数：无
返回值：0表示成功
*/
int schedule_slot(void)
{
    /*主机M*/
    if (MY_INDEX == 0)
    {
        if ( (0 <= info.current_slot <= 28) || (info.current_slot == 61) )
        {
            antenna_table[info.current_antenna] = 1;
            sem_post(&info.send_semaphore);
            udelay(slot_table[info.current_slot]);
            info.current_antenna = (info.current_antenna + 1) % ANTENNA_NUM;
            info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            memset(antenna_table, 0, sizeof(antenna_table));
        }
        else
        {
            udelay(slot_table[info.current_slot]);
            info.current_slot = (info.current_slot + 1) % SLOT_NUM;
        } 
    }
    /*从机Z*/
    else
    {
        switch (MY_INDEX)
        {
        case 1:
            if (30 <= info.current_slot <= 34)
            {
                sem_post(&info.send_semaphore);
                udelay(slot_table[info.current_slot]);
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            else
            {

                udelay(slot_table[info.current_slot]);
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            break;
        case 2:
            if (35 <= info.current_slot <= 39)
            {
                sem_post(&info.send_semaphore);
                udelay(slot_table[info.current_slot]);
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            else
            {
                udelay(slot_table[info.current_slot]);
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            break;
        case 3:
            if ((40 <= info.current_slot <= 43) || (info.current_slot == 30))
            {
                sem_post(&info.send_semaphore);
                udelay(slot_table[info.current_slot]);
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            else
            {
                udelay(slot_table[info.current_slot]);
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            break;
        case 4:
            if ((44 <= info.current_slot <= 47) || (info.current_slot == 35))
            {
                sem_post(&info.send_semaphore);
                udelay(slot_table[info.current_slot]);
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            else
            {
                udelay(slot_table[info.current_slot]);
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            break;
        default:
            break;
        }
    }
    return 0;
}

/*
功能：将当前数据时隙与在网终端Z匹配
参数：索引号，当前数据时隙
返回值：成功则返回对应索引号，失败返回-1
*/
int schedule_inquire_index(int index, int current_slot)
{
    if ((inquire_index(index) == 1) && (0 < current_slot < 5))
    {
        return 1;
    }
    if ((inquire_index(index) == 2) && (5 < current_slot < 10))
    {
        return 2;
    }
    if ((inquire_index(index) == 3) && (9 < current_slot < 14))
    {
        return 3;
    }
    if ((inquire_index(index) == 4) && (13 < current_slot < 18))
    {
        return 4;
    }
    return -1;
}

/*
功能：判断当前终端Z是否在网
参数：节点索引
返回值：在网则返回对应节点索引，不在网则返回-1
*/
int inquire_index(int node_index)
{
    int i = info.device_info.node_list & 30;
    if (i & (1 << node_index))
    {
        return node_index;
    }
    return -1;
}

/*
功能：判断当前时隙属于哪个终端Z的发送时隙
参数：节点索引
返回值：在网则返回对应节点索引，不在网则返回-1
*/
int inquire_slot(int current_slot)
{
    if (29 < current_slot < 35)
    {
        return 1;
    }
    if (34 < current_slot < 40)
    {
        return 2;
    }
    if (39 < current_slot < 44)
    {
        return 3;
    }
    if (43 < current_slot < 48)
    {
        return 4;
    }
    return -1;
}

/*
功能：定时器回调
参数：
返回值：无
*/
void CALLBACK TimerCallback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    switch (dwUser)
    {
    case SCAN_REQ_TIMER:
        printf("Z not receive scan require in 300ms\n");
        break;
    case SCAN_RES_TIMER:
        printf("M not receive scan response in 300ms\n");
        break;
    case SCAN_CON_TIMER:
        printf("Z not receive scan confirm in 300ms\n");
        break;
    default:
        break;
    }
}


//timerId = timeSetEvent(timer, 0, TimerCallback, timer_id, TIME_ONESHOT);
//timeKillEvent(timerId);




//static slot_event_t m1_send_z1[]    = { {800},{800},{800},{800},{800} };
//static slot_event_t m1_send_z2[]    = { {800},{800},{800},{800},{800} };
//static slot_event_t m1_send_z3[]    = { {800},{800},{800},{800} };
//static slot_event_t m1_send_z4[]    = { {800},{800},{800},{800} };
//static slot_event_t m1_send_m2[]    = { {800},{800},{800},{800},{800},{800} };
//static slot_event_t m1_send_z5[]    = { {800},{800},{800},{800},{800} };
//static slot_event_t protect1[]      = { {4000} };
//static slot_event_t z1_send_m1[]    = { {2600},{2600},{2600},{2600},{2600} };
//static slot_event_t z2_send_m1[]    = { {2600},{2600},{2600},{2600},{2600} };
//static slot_event_t z3_send_m1[]    = { {2600},{2600},{2600},{2600} };
//static slot_event_t z4_send_m1[]    = { {2600},{2600},{2600},{2600} };
//static slot_event_t z5_send_m1[]    = { {2600},{2600},{2600},{2600},{2600} };
//static slot_event_t m2_send_m1[]    = { {800},{800},{800},{800},{800},{800} };
//static slot_event_t z_distance_m1[] = { {2600} };
//static slot_event_t protect2[]      = { {4000} };
//static slot_event_t m1_distance_z[] = { {800} };
//static slot_event_t reserve[]       = { {800} };
//
//static slot_table_t slot_table[SLOT_NUM]=
//{
//    {&m1_send_z1,    sizeof(m1_send_z1) / sizeof(m1_send_z1[0]), 0, 0, 0},
//    {&m1_send_z2,    sizeof(m1_send_z2) / sizeof(m1_send_z2[0]), 0, 0, 0},
//    {&m1_send_z3,    sizeof(m1_send_z3) / sizeof(m1_send_z3[0]), 0, 0, 0},
//    {&m1_send_z4,    sizeof(m1_send_z4) / sizeof(m1_send_z4[0]), 0, 0, 0},
//    {&m1_send_m2,    sizeof(m1_send_m2) / sizeof(m1_send_m2[0]), 0, 0, 0},
//    {&m1_send_z5,    sizeof(m1_send_z5) / sizeof(m1_send_z5[0]), 0, 0, 0},
//    {&protect1,      sizeof(protect1) / sizeof(protect1[0]), 0, 0, 0},
//    {&z1_send_m1,    sizeof(z1_send_m1) / sizeof(z1_send_m1[0]), 0, 0, 0},
//    {&z2_send_m1,    sizeof(z2_send_m1) / sizeof(z2_send_m1[0]), 0, 0, 0},
//    {&z3_send_m1,    sizeof(z3_send_m1) / sizeof(z3_send_m1[0]), 0, 0, 0},
//    {&z4_send_m1,    sizeof(z4_send_m1) / sizeof(z4_send_m1[0]), 0, 0, 0},
//    {&z5_send_m1,    sizeof(z5_send_m1) / sizeof(z5_send_m1[0]), 0, 0, 0},
//    {&m2_send_m1,    sizeof(m2_send_m1) / sizeof(m2_send_m1[0]), 0, 0, 0},
//    {&z_distance_m1, sizeof(z_distance_m1) / sizeof(z_distance_m1[0]), 0, 0, 0},
//    {&protect2,      sizeof(protect2) / sizeof(protect2[0]), 0, 0, 0},
//    {&m1_distance_z, sizeof(m1_distance_z) / sizeof(m1_distance_z[0]), 0, 0, 0},
//    {&reserve,       sizeof(reserve) / sizeof(reserve[0]), 0, 0, 0},
//};
//
//
//int schedule_slot_init(void)
//{
//    memset(slot_table, 0, sizeof(slot_table_t));
//    slot_table[0].num = 5;
//    slot_table[1].num = 5;
//    slot_table[2].num = 4;
//    slot_table[3].num = 4;
//    slot_table[4].num = 6;
//    slot_table[5].num = 5;
//    slot_table[6].num = 1;
//    slot_table[7].num = 5;
//    slot_table[8].num = 5;
//    slot_table[9].num = 4;
//    slot_table[10].num = 4;
//    slot_table[11].num = 5;
//    slot_table[12].num = 6;
//    slot_table[13].num = 1;
//    slot_table[14].num = 1;
//    slot_table[15].num = 1;
//    slot_table[16].num = 1; 
//    int i,j;
//    for (i = 0; i < SLOT_NUM; i++)
//    {
//        if (i == 0 || i == 1 || i == 5 || i == 7 || i == 8 || i == 11)
//        {
//            slot_table[i].num = 5;
//        }
//        else if (i == 2 || i == 3 || i == 9 || i == 10)
//        {
//            slot_table[i].num = 4;
//        }
//        else if (i == 4 || i == 12)
//        {
//            slot_table[i].num = 6;
//        }
//        else
//        {
//            slot_table[i].num = 1;
//        }
//    }
//    for (i = 0; i < SLOT_NUM; i++)
//        for (j = 0; j < slot_table[i].num; j++)
//        {
//            if ((0<= i <=5) || i == 12 || i == 15 || i == 16)
//            {
//                slot_table[i].slot[j] = 800;
//            }
//            else if ((6 < i < 12) || i == 13)
//            {
//                slot_table[i].slot[j] = 2600;
//            }
//            else
//            {
//                slot_table[i].slot[j] = 4000;
//            }
//        }
//    return 0;
//}


        //switch (info.current_slot)
        //{
        //case M1_SEND_Z1:
        //case M1_SEND_Z2:
        //case M1_SEND_Z3:
        //case M1_SEND_Z4:
        //    sem_post(&info.send_semaphore);
        //    //udelay(slot_table[info.current_slot]);
        //    break;
        //case M1_SEND_M2:
        //case M1_SNED_Z5:
        //case PROTECT1:
        //case PROTECT2:            
        //    //udelay(slot_table[info.current_slot]);
        //    break;
        //case Z1_SEND_M1:
        //case Z2_SEND_M1:
        //case Z3_SEND_M1:
        //case Z4_SEND_M1:
        //case Z5_SEND_M1:
        //case M2_SEND_M1:           
        //   // udelay(slot_table[info.current_slot]);
        //    break;
        //case Z_DISTANCE_M1:
        //case M1_DISTANCE_Z:            
        //   // udelay(slot_table[info.current_slot]);
        //    break;
        //case RESERVE:
        //    //udelay(slot_table[info.current_slot]);
        //    break;
        //default:
        //    return 0;
        //    break;
        //}

        //switch (info.current_slot)
        //{
        //case M1_SEND_Z1:
        //case M1_SEND_Z2:
        //case M1_SEND_Z3:
        //case M1_SEND_Z4:
        //case M1_SEND_M2:
        //case M1_SNED_Z5:
        //   // udelay(slot_table[info.current_slot]);
        //    break;
        //case PROTECT1:
        //case PROTECT2:
        //   // udelay(slot_table[info.current_slot]);
        //    break;
        //case Z1_SEND_M1:
        //    if ((MY_INDEX + 6) == Z1_SEND_M1)
        //    {
        //        sem_post(&info.send_semaphore);
        //        //udelay(slot_table[info.current_slot]);
        //    }
        //    else
        //    {
        //       // udelay(slot_table[info.current_slot]);
        //    }
        //    break;
        //case Z2_SEND_M1:
        //    if ((MY_INDEX + 6) == Z2_SEND_M1)
        //    {
        //        sem_post(&info.send_semaphore);
        //       //udelay(slot_table[info.current_slot]);
        //    }
        //    else
        //    {
        //       // udelay(slot_table[info.current_slot]);
        //    }
        //    break;
        //case Z3_SEND_M1:
        //    if ((MY_INDEX + 6) == Z3_SEND_M1)
        //    {
        //        sem_post(&info.send_semaphore);
        //        //udelay(slot_table[info.current_slot]);
        //    }
        //    else
        //    {
        //       // udelay(slot_table[info.current_slot]);
        //    }
        //    break;
        //case Z4_SEND_M1:
        //    if ((MY_INDEX + 6) == Z4_SEND_M1)
        //    {
        //        sem_post(&info.send_semaphore);
        //        //udelay(slot_table[info.current_slot]);
        //    }
        //    else
        //    {
        //       // udelay(slot_table[info.current_slot]);
        //    }
        //    break;
        //case Z5_SEND_M1:
        //case M2_SEND_M1:
        //    //udelay(slot_table[info.current_slot]);
        //    break;
        //case Z_DISTANCE_M1:
        //case M1_DISTANCE_Z:
        //    //udelay(slot_table[info.current_slot]);
        //    break;
        //case RESERVE:
        //    //udelay(slot_table[info.current_slot]);
        //    break;
        //default:
        //    return 0;
        //    break;
        //}