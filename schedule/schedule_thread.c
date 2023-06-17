#include "schedule_thread.h"

#define ANTENNA_NUM   6   /*天线数*/
#define SLOT_NUM	  63  /*时隙数*/


static int antenna_table[ANTENNA_NUM] = { 0, 0, 0, 0, 0, 0 };      //0接收1发送

static int slot_table[SLOT_NUM] = 
{
800, 800, 800, 800, 800,        //0-4       M1_SEND_Z1
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
        if ( (0 <= info.current_slot && info.current_slot <= 28) || (info.current_slot == 61) )
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
            if ((30 <= info.current_slot && info.current_slot <= 34) || (info.current_slot == 59))
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
            if ((35 <= info.current_slot && info.current_slot <= 39) || (info.current_slot == 59))
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
            if ((40 <= info.current_slot && info.current_slot <= 43) || (info.current_slot == 30) || (info.current_slot == 59))
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
            if ((44 <= info.current_slot && info.current_slot <= 47) || (info.current_slot == 35) || (info.current_slot == 59))
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
    if ((inquire_index(index) == 1) && (1 <= current_slot && current_slot <= 4))
    {
        return 1;
    }
    if ((inquire_index(index) == 2) && (6 <= current_slot && current_slot <= 9))
    {
        return 2;
    }
    if ((inquire_index(index) == 3) && (10 <= current_slot && current_slot <= 13))
    {
        return 3;
    }
    if ((inquire_index(index) == 4) && (14 <= current_slot && current_slot <= 17))
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
    int i = info.device_info.node_list & (1 << node_index);
    if (i != 0)
    {
        return node_index;
    }
    return -1;
}

/*
功能：判断当前时隙属于哪个终端Z的发送时隙
参数：节点索引
返回值：成功则返回对应节点索引，否则返回-1
*/
int inquire_slot(int current_slot)
{
    if (31 <= current_slot && current_slot <= 34)
    {
        return 1;
    }
    if (36 <= current_slot && current_slot <= 39)
    {
        return 2;
    }
    if (40 <= current_slot && current_slot <= 43)
    {
        return 3;
    }
    if (44 <= current_slot && current_slot <= 47)
    {
        return 4;
    }
    return -1;
}

/*
功能：判断当前接收时隙属于终端Z的哪个天线
参数：当前时隙
返回值：天线索引
*/
int inquire_antenna(int current_slot)
{
    if ((0 <= current_slot && current_slot <= 6) || (42 <= current_slot && current_slot <= 48))
    {
        return 0;
    }
    if ((7 <= current_slot && current_slot <= 13) || (49 <= current_slot && current_slot <= 55))
    {
        return 1;
    }
    if ((14 <= current_slot && current_slot <= 20) || (56 <= current_slot && current_slot <= 62))
    {
        return 2;
    }
    if (21 <= current_slot && current_slot <= 27)
    {
        return 3;
    }
    if (28 <= current_slot && current_slot <= 29)
    {
        return 4;
    }
    if (35 <= current_slot && current_slot <= 41)
    {
        return 5;
    }

    return -1;
}

/*
功能：根据节点地址判断节点索引
参数：节点地址
返回值：节点索引
*/
int inquire_address(int node_id)
{
    int index;
    index = node_id & 15;
    return index;
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
    case DATA_TIMER:
        printf("M not receive Z message in 300ms\n");
        break;
    default:
        break;
    }
}
