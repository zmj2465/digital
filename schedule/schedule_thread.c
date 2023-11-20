#include "schedule_thread.h"

#define ANTENNA_NUM   6   /*天线数*/
#define SLOT_NUM	  63  /*时隙数*/



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
            info.data_store_flag = 1;
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
    info.chain_antenna = 1;
    info.current_time_frame = 0;
    info.scan_flag_Z = 0;
    memset(info.distance_flag_M, 0, sizeof(info.distance_flag_M));
    memset(info.scan_flag_M, 0, sizeof(info.scan_flag_M));
	memset(info.znode_connect_flag,0,sizeof(info.znode_connect_flag));
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
            info.time_schedule_flag = 1;
            udelay(slot_table[info.current_slot]);
            display_data.antenna_params[info.current_antenna].tx_rx_status = 0;//将上次时隙中的天线置0，即关闭状态，并推送到显控界面
            info.chain_antenna = (info.chain_antenna + 1) % ANTENNA_NUM;
            info.current_slot = (info.current_slot + 1) % SLOT_NUM;
        }
        else
        {
            udelay(slot_table[info.current_slot]);
            display_data.antenna_params[info.current_antenna].tx_rx_status = 0;//将上次时隙中的天线置0，即关闭状态，并推送到显控界面
            info.current_slot = (info.current_slot + 1) % SLOT_NUM;
        }
        if (info.current_slot == 0)//每100ms增加1个时帧号
        {
            info.current_time_frame++;//协议内部的时帧号
            display_data.time_frame_number = (display_data.time_frame_number + 1) % 10;//推送给显控的时帧号
            if (display_data.time_frame_number == 0)
            {
                display_data.time_element_number++;//时帧号满10进1，增加1个时元号
            }
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
                info.time_schedule_flag = 1;
                udelay(slot_table[info.current_slot]);
                display_data.antenna_params[info.current_antenna].tx_rx_status = 0;//将上次时隙中的天线置0，即关闭状态，并推送到显控界面
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            else
            {
                udelay(slot_table[info.current_slot]);
                display_data.antenna_params[info.current_antenna].tx_rx_status = 0;//将上次时隙中的天线置0，即关闭状态，并推送到显控界面
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            if (info.current_slot == 0)//每100ms增加1个时帧号
            {
                info.current_time_frame++;//协议内部的时帧号
                display_data.time_frame_number = (display_data.time_frame_number + 1) % 10;
                if (display_data.time_frame_number == 0)
                {
                    display_data.time_element_number++;//时帧号满10进1，增加1个时元号
                }
            }
            break;
        case 2:
            if ((35 <= info.current_slot && info.current_slot <= 39) || (info.current_slot == 59))
            {
                info.time_schedule_flag = 1;
                udelay(slot_table[info.current_slot]);
                display_data.antenna_params[info.current_antenna].tx_rx_status = 0;//将上次时隙中的天线置0，即关闭状态，并推送到显控界面
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            else
            {
                udelay(slot_table[info.current_slot]);
                display_data.antenna_params[info.current_antenna].tx_rx_status = 0;//将上次时隙中的天线置0，即关闭状态，并推送到显控界面
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            if (info.current_slot == 0)//每100ms增加1个时帧号
            {
                info.current_time_frame++;//协议内部的时帧号
                display_data.time_frame_number = (display_data.time_frame_number + 1) % 10;
                if (display_data.time_frame_number == 0)
                {
                    display_data.time_element_number++;//时帧号满10进1，增加1个时元号
                }
            }
            break;
        case 3:
            if ((40 <= info.current_slot && info.current_slot <= 43) || (info.current_slot == 59) || (info.current_slot == 30))
            {
                info.time_schedule_flag = 1;
                udelay(slot_table[info.current_slot]);
                display_data.antenna_params[info.current_antenna].tx_rx_status = 0;//将上次时隙中的天线置0，即关闭状态，并推送到显控界面
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            else
            {
                udelay(slot_table[info.current_slot]);
                display_data.antenna_params[info.current_antenna].tx_rx_status = 0;//将上次时隙中的天线置0，即关闭状态，并推送到显控界面
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            if (info.current_slot == 0)//每100ms增加1个时帧号
            {
                info.current_time_frame++;//协议内部的时帧号
                display_data.time_frame_number = (display_data.time_frame_number + 1) % 10;
                if (display_data.time_frame_number == 0)
                {
                    display_data.time_element_number++;//时帧号满10进1，增加1个时元号
                }
            }
            break;
        case 4:
            if ((44 <= info.current_slot && info.current_slot <= 47) || (info.current_slot == 59) || (info.current_slot == 35))
            {
                info.time_schedule_flag = 1;
                udelay(slot_table[info.current_slot]);
                display_data.antenna_params[info.current_antenna].tx_rx_status = 0;//将上次时隙中的天线置0，即关闭状态，并推送到显控界面
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            else
            {
                udelay(slot_table[info.current_slot]);
                display_data.antenna_params[info.current_antenna].tx_rx_status = 0;//将上次时隙中的天线置0，即关闭状态，并推送到显控界面
                info.current_slot = (info.current_slot + 1) % SLOT_NUM;
            }
            if (info.current_slot == 0)//每100ms增加1个时帧号
            {
                info.current_time_frame++;//协议内部的时帧号
                display_data.time_frame_number = (display_data.time_frame_number + 1) % 10;
                if (display_data.time_frame_number == 0)
                {
                    display_data.time_element_number++;//时帧号满10进1，增加1个时元号
                }
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
参数：索引号，当前时隙
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
功能：将当前信令时隙与终端Z匹配，奇数时帧，Z1、Z2给M发信令，偶数时帧，Z3、Z4给M发信令
参数：索引号，当前时隙，当前时帧
返回值：成功则返回对应索引号，失败返回-1
*/
int beacon_z_inquire_index(int index, int slot, int time_frame)
{
    if ((index == 1) && (slot == 30) && (time_frame % 2 == 1))
    {
        return 1;
    }
    if ((index == 2) && (slot == 35) && (time_frame % 2 == 1))
    {
        return 2;
    }
    if ((index == 3) && (slot == 30) && (time_frame % 2 == 0))
    {
        return 3;
    }
    if ((index == 4) && (slot == 35) && (time_frame % 2 == 0))
    {
        return 4;
    }
    return -1;
}

/*
功能：M将当前信令时隙与在网终端Z匹配，奇数时帧，M给Z1、Z2发信令，偶数时帧，M给Z3、Z4发信令
参数：当前时隙，当前时帧
返回值：成功则返回对应索引号，失败返回-1
*/
int beacon_m_inquire_index(int index, int slot, int time_frame)
{
    if ((inquire_index(index) == 1) && (slot == 0) && (time_frame % 2 == 1))
    {
        return 1;
    }
    if ((inquire_index(index) == 2) && (slot == 5) && (time_frame % 2 == 1))
    {
        return 2;
    }
    if ((inquire_index(index) == 3) && (slot == 0) && (time_frame % 2 == 0))
    {
        return 3;
    }
    if ((inquire_index(index) == 4) && (slot == 5) && (time_frame % 2 == 0))
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
功能：判断当前时隙是哪个Z的发送时隙
参数：索引号，当前时隙
返回值：成功则返回对应索引号，失败返回-1
*/
int inquire_node_index(int index, int current_slot)
{
    if ((index == 1) && (31 <= current_slot && current_slot <= 34))
    {
        return 1;
    }
    if ((index == 2) && (36 <= current_slot && current_slot <= 39))
    {
        return 2;
    }
    if ((index == 3) && (40 <= current_slot && current_slot <= 43))
    {
        return 3;
    }
    if ((index == 4) && (44 <= current_slot && current_slot <= 47))
    {
        return 4;
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
参数：定时器Id
返回值：无
*/
#ifdef _WIN32
void CALLBACK TimerCallback(UINT uID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
    switch (dwUser)
    {
    case 1:
        plog("M not receive Z1 beacon in 300ms\n");
        info.device_info.node_num--;
        info.device_info.node_list = info.device_info.node_list ^ (1 << 1);
        plog("M lost Z1 , list = %d, current slot = %d.%d\n", info.device_info.node_list, info.current_time_frame, info.current_slot);
        if (fsm_status == FSM_ON)
        {
            fsm_do(EVENT_LOST_Z);
        }
        break;
    case 2:
        plog("M not receive Z2 beacon in 300ms\n");
        info.device_info.node_num--;
        info.device_info.node_list = info.device_info.node_list ^ (1 << 2);
        plog("M lost Z2 , list = %d, current slot = %d.%d\n", info.device_info.node_list, info.current_time_frame, info.current_slot);
        if (fsm_status == FSM_ON)
        {
            fsm_do(EVENT_LOST_Z);
        }
        break;
    case 3:
        plog("M not receive Z3 beacon in 300ms\n");
        info.device_info.node_num--;
        info.device_info.node_list = info.device_info.node_list ^ (1 << 3);
        plog("M lost Z3 , list = %d, current slot = %d.%d\n", info.device_info.node_list, info.current_time_frame, info.current_slot);
        if (fsm_status == FSM_ON)
        {
            fsm_do(EVENT_LOST_Z);
        }
        break;
    case 4:
        plog("M not receive Z4 beacon in 300ms\n");
        info.device_info.node_num--;
        info.device_info.node_list = info.device_info.node_list ^ (1 << 4);
        plog("M lost Z4 , list = %d, current slot = %d.%d\n", info.device_info.node_list, info.current_time_frame, info.current_slot);
        if (fsm_status == FSM_ON)
        {
            fsm_do(EVENT_LOST_Z);
        }
        break;
    case SCAN_REQ_TIMER:
        plog("Z not receive scan require in 300ms\n");
        break;
    case SCAN_RES_TIMER:
        plog("M not receive scan response in 300ms\n");
        break;
    case SCAN_CON_TIMER:
        plog("Z not receive scan confirm in 300ms\n");
        break;
    case Z_DATA_TIMER:
        plog("Z not receive M beacon in 300ms\n");
        fsm_do(EVENT_LOST_M);
        break;
    case START_GUN_TIMER:
        plog("M not receive start gun response\n");
        msg_t msg;
        int i;
        msg.data[0] = START_GUN_REQ;
        msg.data[1] = START_GUN_TIME;
        msg.len = 2;
        info.str.base_t = my_get_time();
        info.str.start_time = START_GUN_TIME;
        for (i = 1; i < FD_NUM; i++)
        {
            generate_packet(info.device_info.node_id[i], info.device_info.node_id[MY_INDEX], START_GUN, &msg);
            send(FD[i].fd, &msg, msg.len, 0);
        }
        printf("M base time=%lld ns, start_time = %d s\n", info.str.base_t, info.str.start_time);
#ifdef _WIN32
        info.timerId = timeSetEvent(TIMER_DELAY, 0, TimerCallback, START_GUN_TIMER, TIME_ONESHOT);
#endif
        break;
    default:
        break;
    }
}

/*
功能：设置天线的参数信息：天线状态，波束宽度，EIRP，GT
参数：对应天线ID，i节点的索引值（0-4：1M4Z），天线状态信息（0：关闭，1：发送，2：接收）
返回值：自身的波束宽度
*/
int set_antenna_parameter(int antenna_id, int index, int antenna_status)
{
    double dis;
    dis = caculate_distance(fddi_info.pos, overall_fddi_info[index].pos);//计算index节点相对于本节点的距离
    
    if (antenna_status == 0)
    {
        display_data.antenna_params[antenna_id].tx_rx_status = antenna_status;
    }
    else
    {
        display_data.antenna_params[antenna_id].tx_rx_status = antenna_status;
        if (MY_INDEX == 0)//本节点是M
        {
            if (0 <= dis && dis < 500)
            {
                display_data.antenna_params[antenna_id].beam_width = 55;
                display_data.antenna_params[antenna_id].eirp = 24;
                display_data.antenna_params[antenna_id].gt = -36;
            }
            else if (500 <= dis && dis < 30000)
            {
                display_data.antenna_params[antenna_id].beam_width = 26;
                display_data.antenna_params[antenna_id].eirp = 46;
                display_data.antenna_params[antenna_id].gt = -21;
            }
            else if (30000 <= dis && dis < 500000)
            {
                display_data.antenna_params[antenna_id].beam_width = 9;
                display_data.antenna_params[antenna_id].eirp = 72;
                display_data.antenna_params[antenna_id].gt = -6;
            }
            else
            {
                display_data.antenna_params[antenna_id].beam_width = 0;
                display_data.antenna_params[antenna_id].eirp = 0;
                display_data.antenna_params[antenna_id].gt = 0;
            }
        }
        else//本节点是Z
        {
            if (0 <= dis && dis < 5000)
            {
                display_data.antenna_params[antenna_id].beam_width = 55;
                display_data.antenna_params[antenna_id].eirp = 24;
                display_data.antenna_params[antenna_id].gt = -36;
            }
            else if (5000 <= dis && dis < 15000)
            {
                display_data.antenna_params[antenna_id].beam_width = 50;
                display_data.antenna_params[antenna_id].eirp = 33;
                display_data.antenna_params[antenna_id].gt = -29;
            }
            else if (15000 <= dis && dis < 30000)
            {
                display_data.antenna_params[antenna_id].beam_width = 26;
                display_data.antenna_params[antenna_id].eirp = 46;
                display_data.antenna_params[antenna_id].gt = -21;
            }
            else if (30000 <= dis && dis < 500000)
            {
                display_data.antenna_params[antenna_id].beam_width = 19;
                display_data.antenna_params[antenna_id].eirp = 55;
                display_data.antenna_params[antenna_id].gt = -15;
            }
            else
            {
                display_data.antenna_params[antenna_id].beam_width = 0;
                display_data.antenna_params[antenna_id].eirp = 0;
                display_data.antenna_params[antenna_id].gt = 0;
            }
        }
    }
    
    return 0;
}

#endif