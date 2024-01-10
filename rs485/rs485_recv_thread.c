#include "rs485_recv_thread.h"
#include "xdmaDLL_public.h"
#include "comcal_dll.h"
#include "display_send_thread.h"
#include "data_send_thread.h"
#include "stddef.h"
#include <string.h>
#include "crc.h"
#include "log.h"

#define MSGLEN   4
#define FLAG_LEN 1
#define HEAD_LEN 3
#define CC_LEN   100

static pthread_mutex_t pcie_mutex;

static int frameseq = 0;

char path[1024];

static HANDLE recv_hdev;
static HANDLE send_hdev;
static HANDLE user_hdev;
static int rs485_fd;
static int lfd;

static int id_table[5] = { 0x10,0x11,0x12,0x13,0x14 };

#define PRINT 
//#define RS485

static int get_seq = 0;


void* rs_485_recv_thread(void* arg)
{
    pthread_detach(pthread_self());
    //int reta = 0;
    //int retb = 0;
    int ret;
    int i;
    uint8_t data[2048];
#ifdef RS485
    ret = rs485_init();
    if (ret < 0) return 0;
    Sleep(1000);
    pcie_check();
#else
    server_init();
#endif
    //printf("init ok\n");
    display_data.micro_time_slot_number = 1;
    //create_CheckResult_res();
    Sleep(100);
    create_ConfigLoad_res();
    Sleep(100);
    create_WorkMode_res();
    Sleep(100);
    create_LinkResult_res();
    Sleep(100);
    create_ShortFrame_res();
    Sleep(2000);

    while (1)
    {
        memset(data, 0, sizeof(data));
#ifdef RS485
        //printf("ready for recv\n");
        ret = read_device(recv_hdev, 0x10000000, 2048, data);
        ret = last_packetSize(user_hdev);
        //Sleep(20);
        
#ifdef PRINT
        p_log_string(0, "--------------------------\n");
        p_log_data(0, ret, data);

#endif
#else
        ret = recv(rs485_fd, data, 2048, 0);
        if (ret <= 0)
        {
            rs485_fd = accept(lfd, (struct sockaddr*)&(info.control_system.addr), &(info.control_system.addr_len));//接收连接请求
            printf("rs485 connect success %d\n", rs485_fd);
            continue;
        }
        p_log_string(0, "get--------------------------\n");
        p_log_data(0, ret, data);
        //for (i = 0; i < ret; i++)
        //{
        //    printf("%02x ", (uint8_t)data[i]);
        //}
        //printf("\n");
#endif
        
        //printf("-----get 485 data len=%d\n", ret);
        rs_head_t* head = (rs_head_t*)data;
        display_data.rx_timestamp = my_get_time();

        if (head->address_a == 0xaa && head->address_b == 0xbb)
        {
            rs_communicate_enalbe_proc(data);
            continue;
        }
        printf("-----get 485 data len=%d  get_seq=%d\n", ret, get_seq++);
        //for (i = 0; i < 2048; i++)
        //{
        //    printf("%02x ", (uint8_t)data[i]);
        //}
        //printf("\n");

        //M
        if (head->type == RS_M2Z_GUI)
        {
            printf("recv type=RS_M2Z_GUI \n");
#ifdef PRINT
            p_log_string(0, "recv type=RS_M2Z_GUI\n");
#endif
			info.m_proc_flight_control_data_rx_count++;
			rs_M2ZGui_proc(data);
            continue;
        }
        else if (head->type == RS_M2Z_TOM)
        {
            printf("recv type=RS_M2Z_TOM \n");
#ifdef PRINT
            p_log_string(0, "recv type=RS_M2Z_TOM\n");
#endif
			info.m_proc_flight_control_data_rx_count++;
			rs_M2ZTom_proc(data);
            continue;
        }
        else if (head->type == RS_M2Z_PLAN)
        {
            printf("recv type=RS_M2Z_PLAN ");
			info.m_proc_flight_control_data_rx_count++;
            rs_M2ZPlan_proc(data);
            continue;
        }

        //Z-M
        switch (head->typea)
        {
        case RS_SELF_CHECK: //自检
			if (0 == MY_INDEX)
			{
				info.m_proc_flight_control_data_rx_count++;
			}
			else
			{
				info.z_proc_flight_control_data_rx_count++;
			}
#ifdef PRINT
            p_log_string(0, "recv type=RS_SELF_CHECK\n");
#endif
            printf("recv type=RS_SELF_CHECK ");
            rs_SelfCheck_proc(data);
            break;
        case RS_SLEF_CHECK_RESULT: //自检结果
			if (0 == MY_INDEX)
			{
				info.m_proc_flight_control_data_rx_count++;
			}
			else
			{
				info.z_proc_flight_control_data_rx_count++;
			}
#ifdef PRINT
            p_log_string(0, "recv type=RS_SLEF_CHECK_RESULT\n");
#endif
            printf("recv type=RS_SLEF_CHECK_RESULT ");
            rs_SelfCheckResult_proc(data);
            break;
        case RS_CONFIG_LOAD: //参数装订
			if (0 == MY_INDEX)
			{
				info.m_proc_flight_control_data_rx_count++;
			}
			else
			{
				info.z_proc_flight_control_data_rx_count++;
			}
#ifdef PRINT
            p_log_string(0, "recv type=RS_CONFIG_LOAD\n");
#endif
            printf("recv type=RS_CONFIG_LOAD ");
            rs_ConfigLoad_proc(data);
            break;
        case RS_SHORT_FRAME: //短帧
            //printf("recv type=RS_SHORT_FRAME ");
#ifdef PRINT
            p_log_string(0, "recv type=RS_SHORT_FRAME\n");
#endif
            rs_ShortFrame_proc(data);
            break;
        case RS_LONG_FRAME: //长帧
			if (0 == MY_INDEX)
			{
				info.m_proc_flight_control_data_rx_count++;
			}
			else
			{
				info.z_proc_flight_control_data_rx_count++;
			}
            printf("recv type=RS_LONG_FRAME \n");
#ifdef PRINT
            p_log_string(0, "recv type=RS_LONG_FRAME\n");
#endif
            rs_LongFrame_proc(data,ret);
            break;
        case RS_START_LINK: //建链
			if (0 == MY_INDEX)
			{
				info.m_proc_flight_control_data_rx_count++;
			}
			else
			{
				info.z_proc_flight_control_data_rx_count++;
			}
#ifdef PRINT
            p_log_string(0, "recv type=RS_START_LINK\n");
#endif
            printf("recv type=RS_START_LINK ");
            rs_Link_proc(data);
            break;
        case RS_LINK_RESULT: //建链结果
			if (0 == MY_INDEX)
			{
				info.m_proc_flight_control_data_rx_count++;
			}
			else
			{
				info.z_proc_flight_control_data_rx_count++;
			}
#ifdef PRINT
            p_log_string(0, "recv type=RS_LINK_RESULT\n");
#endif
            printf("recv type=RS_LINK_RESULT ");
            rs_Link_result_proc(data);
            break;
        case RS_WORK_MODE: //工作模式
			if (0 == MY_INDEX)
			{
				info.m_proc_flight_control_data_rx_count++;
			}
			else
			{
				info.z_proc_flight_control_data_rx_count++;
			}
#ifdef PRINT
            p_log_string(0, "recv type=RS_WORK_MODE\n");
#endif
            printf("recv type=RS_WORK_MODE ");
            work_mode_proc(data);
            break;
        case RS_PRE_SEPARATE: //预分离
			if (0 == MY_INDEX)
			{
				info.m_proc_flight_control_data_rx_count++;
			}
			else
			{
				info.z_proc_flight_control_data_rx_count++;
			}
#ifdef PRINT
            p_log_string(0, "recv type=RS_PRE_SEPARATE\n");
#endif
            printf("recv type=RS_PRE_SEPARATE ");
            rs_PreSeparate_proc(data);
            break;
        }
    }
}


void send_to_rs(int len, long offset, char* data)
{
    int ret = 0;
    int i;
    rs_head_t* rhead = (rs_head_t*)data;

    display_data.tx_timestamp = my_get_time();

    

#ifdef RS485
    //if (rhead->typea != 0x30) 
    {
        pthread_mutex_lock(&pcie_mutex);
        ret = write_device(send_hdev, offset, len, data);
        pthread_mutex_unlock(&pcie_mutex);
    }
    //ret = write_device(send_hdev, offset, len, data);

#else
    ret = send(rs485_fd, data, len, 0);
#endif
    //if(ret!= len)

	if (0 == MY_INDEX)
	{
		info.m_proc_flight_control_data_tx_count++;
	}
	else
	{
		info.z_proc_flight_control_data_tx_count++;
	}
#ifdef PRINT
    p_log_string(0, "type %x len=%d send ok\n", rhead->typea, len);
    p_log_data(0, len, data);
#endif
    //for (i = 0; i < len; i++)
    //{
    //    printf("%02x ", (uint8_t)data[i]);
    //}
    //printf("\n");
    if (rhead->typea == 0x30) return;
    printf("type %x len=%d send ok\n", rhead->typea, len);

    //pthread_mutex_unlock(&pcie_mutex);
}

//自检命令回复
void rs_SelfCheck_proc(char* data)
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    uint16_t crc;
    rs_head_t* head = (rs_head_t*)data;
    rs_body_t* body = (rs_body_t*)(data + RS_HEAD_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);
    //crc校验
    if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN, body->body_tail.crc) == 1)
    {
        //printf("self check crc fail\n");
        return;
    }

    //装载头部
    head_load(data, res);

    memcpy(&rbody->body_tail, &body->body_tail, RS_TAIL_LEN);

    rbody->body_tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN);

    send_to_rs(RS_SELF_CHECK_LEN, 0, res);
    generate_key_event(KEY_SELF_CHECK,0,0);
}

//自检结果下传1回复
void rs_SelfCheckResult_proc(char* data)
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* head = (rs_head_t*)data;
    rs_body_t* body = (rs_body_t*)(data + RS_HEAD_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);
    //crc校验
    if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN, body->body_tail.crc) == 1)
    {
        //printf("self check result crc fail\n");
        return;
    }
    head_load(data, res);

    if (MY_INDEX == 0)
    {
        rbody->m_result1_ack.ack = 0;
        rbody->m_result1_ack.version[0] = 0x02;
        rbody->m_result1_ack.version[1] = 0x41;
        rbody->m_result1_ack.version[2] = 0x01;
        rbody->m_result1_ack.version[3] = 0x02;
        rbody->m_result1_ack.version[4] = 0x41;
        rbody->m_result1_ack.version[5] = 0x01;
        rbody->m_result1_ack.version[6] = 0x02;
        rbody->m_result1_ack.version[7] = 0x41;
        rbody->m_result1_ack.version[8] = 0x01;
    memcpy(&rbody->m_result1_ack.tail, &body->body_tail, RS_TAIL_LEN);

    rbody->m_result1_ack.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + 2+9);
    send_to_rs(RS_RESULT_LEN+9, 0, res);
	}
    else
    {
        rbody->z_result1_ack.ack = 0;
        rbody->z_result1_ack.version[0] = 0x02;
        rbody->z_result1_ack.version[1] = 0x41;
        rbody->z_result1_ack.version[2] = 0x01;
        rbody->z_result1_ack.version[3] = 0x02;
        rbody->z_result1_ack.version[4] = 0x41;
        rbody->z_result1_ack.version[5] = 0x01;
        memcpy(&rbody->z_result1_ack.tail, &body->body_tail, RS_TAIL_LEN);

        rbody->z_result1_ack.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + 2+6);
        send_to_rs(RS_RESULT_LEN+6, 0, res);
    }
}


//参数装订回复
void rs_ConfigLoad_proc(char* data)
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* head = (rs_head_t*)data;
    rs_body_t* body = (rs_body_t*)(data + RS_HEAD_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);
    //crc校验
    if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN + CONFIG_INFO_LEN, body->config_load.tail.crc) == 1)
    {
        //printf("config load crc fail\n");
        return;
    }

    head_load(data, res);

    memcpy(&rbody->config_load, &body->config_load, CONFIG_INFO_LEN + RS_TAIL_LEN);

    rbody->config_load.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + CONFIG_INFO_LEN);
    send_to_rs(RS_CONFIG_LEN, 0, res);

    //MY_INDEX = body->config_load.node_id;
    //FD_NUM = body->config_load.node_num;
    
    int i = 0;
    int x = body->config_load.node_list;
    int z_num = 0;
    int m_num = 0;
    for (i = 0; i < 4; i++)
    {
        if((x&(1<<i))==1) z_num++;
    }
    if ((x & (1 << 4)) == 1) m_num++;

    MY_ID = body->config_load.node_id;
    MY_ID_INDEX = MY_ID - 0x10;
    generate_key_event(KEY_CONFIG_LOAD, 1, 1);

    if (MY_INDEX != 0)
    {
        msg_t msg;
        msg.data[0] = MY_INDEX;
        msg.data[1] = MY_ID;
        msg.len = 2;
        generate_packet(info.device_info.node_id[0], MY_ID, PARAMETER_LOAD, &msg);
        send(FD[0].fd, &msg, sizeof(msg_t), 0);
    }

}



//工作模式回复
void work_mode_proc(char* data)
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* head = (rs_head_t*)data;
    rs_body_t* body = (rs_body_t*)(data + RS_HEAD_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);
    //crc校验
    uint16_t get_crc = body->work_mode.tail.crc;
    if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN + 2, get_crc) == 1)
    {
        //printf("work mode crc fail\n");
        return;
    }
    //头部
    head_load(data, res);
    //内容
    
    //尾部
    memset(rbody->work_modes_sp.tail.flag, 0x7e, 4);
    //crc
    rbody->work_modes_sp.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + 1);

    send_to_rs(RS_WORK_MODE_SP_LEN, 0, res);
}

//建链回复
void rs_Link_proc(char* data)
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* head = (rs_head_t*)data;
    rs_body_t* body = (rs_body_t*)(data + RS_HEAD_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);
    //crc校验
    uint16_t get_crc = body->body_tail.crc;
    if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN, get_crc) == 1)
    {
        //printf("link start crc fail\n");
        return;
    }
    //头部
    head_load(data, res);
    //内容
    
    //尾部
    memset(rbody->body_tail.flag, 0x7e, 4);
    //crc
    rbody->body_tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN);

    send_to_rs(RS_LINK_START_LEN, 0, res);

	if (MY_INDEX == 0)
	{
		generate_key_event(5, 0, 0);
		info.chain_flag_m = 1;
	}
	else
	{
		generate_key_event(5, 1, 1);
		info.chain_flag_z = 1;
	}

}

//建链结果回复
void rs_Link_result_proc(char* data)
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* head = (rs_head_t*)data;
    rs_body_t* body = (rs_body_t*)(data + RS_HEAD_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);
    //crc校验
    uint16_t get_crc = body->body_tail.crc;
    if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN, get_crc) == 1)
    {
        //printf("link result crc fail\n");
        return;
    }
    //头部
    head_load(data, res);
    //内容
    
    //尾部
    memset(rbody->link_result_sp.tail.flag, 0x7e, 4);
    //crc加载
    rbody->link_result_sp.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + LINK_RESULT_SP_LEN);

    send_to_rs(RS_LINK_RESULT_SP_LEN, 0, res);
}

//预分离
void rs_PreSeparate_proc(char* data)
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* head = (rs_head_t*)data;
    rs_body_t* body = (rs_body_t*)(data + RS_HEAD_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);
    //crc校验
    uint16_t get_crc = body->body_tail.crc;
    if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN, get_crc) == 1)
    {
        //printf("pre separate crc fail\n");
        return;
    }
    //头部
    head_load(data, res);

    //内容

    //尾部
    memset(rbody->body_tail.flag, 0x7e, 4);
    //crc加载
    rbody->body_tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN);

    send_to_rs(RS_PRE_SEPARATE_SP_LEN, 0, res);

    generate_key_event(KEY_PRE_SEPARATE, 0, 0);
}



//短帧回复
void rs_ShortFrame_proc(char* data)
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* head = (rs_head_t*)data;
    rs_body_t* body = (rs_body_t*)(data + RS_HEAD_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);
    int ant_id=0;
    float distance=0;
    float azimuth=0;
    float elevation=0;

    //display_data.micro_time_slot_number *= 2;

    //头部加载
    head_load(data, res);

    if (MY_INDEX == 0) //M
    { 
		info.m_proc_flight_control_data_rx_count++;
        //crc校验
        uint16_t get_crc = *(uint16_t*)((uint8_t*)&body->m_short_frmae + M_SHORT_FRAME_LEN);
        if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN + M_SHORT_FRAME_LEN, get_crc) == 1)
        {
            //printf("m short frame crc fail\n");
            return;
        }
        //内容
		//memset((uint8_t*)&rbody->m_short_frmae_sp,0,158);

        //rbody->m_short_frmae_sp.time_ljqi = my_get_time();
        //rbody->m_short_frmae_sp.posX = overall_fddi_info[0].pos.x;
        //rbody->m_short_frmae_sp.posY = overall_fddi_info[0].pos.y;
        //rbody->m_short_frmae_sp.posZ = overall_fddi_info[0].pos.z;
        //rbody->m_short_frmae_sp.Vx = overall_fddi_info[0].v.x;
        //rbody->m_short_frmae_sp.Vy = overall_fddi_info[0].v.y;
        //rbody->m_short_frmae_sp.Vz = overall_fddi_info[0].v.z;
        //rbody->m_short_frmae_sp.time_element_number = display_data.time_element_number;
        //rbody->m_short_frmae_sp.time_frame_number = display_data.time_frame_number;
        //rbody->m_short_frmae_sp.micro_time_slot_number = display_data.micro_time_slot_number;
        //rbody->m_short_frmae_sp.node_id = id_table[MY_INDEX];
        //rbody->m_short_frmae_sp.link_status = info.znode_connect_flag[0] | (info.znode_connect_flag[1] << 1) | (info.znode_connect_flag[2] << 2) | (info.znode_connect_flag[3] << 3);//目前只有z1，为0001
        //rbody->m_short_frmae_sp.comm_status_mode = display_data.comm_status_mode;
        //rbody->m_short_frmae_sp.m_proc_flight_control_data_tx_count = info.m_proc_flight_control_data_tx_count;
        //rbody->m_short_frmae_sp.m_proc_flight_control_data_rx_count = info.m_proc_flight_control_data_rx_count;
        //rbody->m_short_frmae_sp.m_proc_flight_control_data_tx_timestamp = display_data.tx_timestamp;
        //rbody->m_short_frmae_sp.m_proc_flight_control_data_rx_timestamp = display_data.rx_timestamp;
        //rbody->m_short_frmae_sp.m_z1_air_interface_data_tx_count = g_node_progrm[1].air_interface_data_tx_count;     //CANG终端向QI1终端发送空口业务数据包计数
        //rbody->m_short_frmae_sp.m_z1_air_interface_data_rx_count = g_node_progrm[1].air_interface_data_rx_count;     //CANG终端向QI1终端接收空口业务数据包计数
        //rbody->m_short_frmae_sp.m_z2_air_interface_data_tx_count = g_node_progrm[2].air_interface_data_tx_count;    //CANG终端向QI2终端发送空口业务数据包计数
        //rbody->m_short_frmae_sp.m_z2_air_interface_data_rx_count = g_node_progrm[2].air_interface_data_rx_count;     //CANG终端向QI2终端接收空口业务数据包计数
        //rbody->m_short_frmae_sp.m_z3_air_interface_data_tx_count = g_node_progrm[3].air_interface_data_tx_count;  //CANG终端向QI3终端发送空口业务数据包计数
        //rbody->m_short_frmae_sp.m_z3_air_interface_data_rx_count = g_node_progrm[3].air_interface_data_rx_count;     //CANG终端向QI3终端接收空口业务数据包计数
        //rbody->m_short_frmae_sp.m_z4_air_interface_data_tx_count = g_node_progrm[4].air_interface_data_tx_count;   //CANG终端向QI4终端发送空口业务数据包计数
        //rbody->m_short_frmae_sp.m_z4_air_interface_data_rx_count = g_node_progrm[4].air_interface_data_rx_count;     //CANG终端向QI4终端接收空口业务数据包计数
        //rbody->m_short_frmae_sp.channel_coding_frame_count=display_data.channel_coding_frame_count;       //信道编码帧计数
        //rbody->m_short_frmae_sp.channel_decoding_frame_count= display_data.channel_decoding_frame_count;     //信道译码帧计数
        //rbody->m_short_frmae_sp.modulation_frame_count= display_data.modulation_frame_count;           //调制帧计数
        //rbody->m_short_frmae_sp.demodulation_frame_count= display_data.demodulation_frame_count;         //解调帧计数
        //rbody->m_short_frmae_sp.instruction_parsing_frame_count = display_data.instruction_parsing_frame_count;           // 指令解析帧计数
        //distance = caculate_distance(overall_fddi_info[0].pos, overall_fddi_info[1].pos);
        //calculate_ante_angle_coord_m(
        //    overall_fddi_info[0].pos.x,
        //    overall_fddi_info[0].pos.y,
        //    overall_fddi_info[0].pos.z,
        //    overall_fddi_info[0].q.q0,
        //    overall_fddi_info[0].q.q1,
        //    overall_fddi_info[0].q.q2,
        //    overall_fddi_info[0].q.q3,
        //    0,
        //    overall_fddi_info[1].pos.x,
        //    overall_fddi_info[1].pos.y,
        //    overall_fddi_info[1].pos.z,
        //    &ant_id,
        //    &azimuth,
        //    &elevation
        //    );
        //rbody->m_short_frmae_sp.m_z1_distance = distance;                    //Cang与Z1距离
        //rbody->m_short_frmae_sp.m_z1_fangwei = azimuth;                    //CANG终端与QI1终端波束方位指向
        //rbody->m_short_frmae_sp.m_z1_fuyang = elevation;                     //CANG终端与QI1终端波束俯仰指向
        //distance = caculate_distance(overall_fddi_info[0].pos, overall_fddi_info[2].pos);
        //calculate_ante_angle_coord_m(
        //    overall_fddi_info[0].pos.x,
        //    overall_fddi_info[0].pos.y,
        //    overall_fddi_info[0].pos.z,
        //    overall_fddi_info[0].q.q0,
        //    overall_fddi_info[0].q.q1,
        //    overall_fddi_info[0].q.q2,
        //    overall_fddi_info[0].q.q3,
        //    0,
        //    overall_fddi_info[2].pos.x,
        //    overall_fddi_info[2].pos.y,
        //    overall_fddi_info[2].pos.z,
        //    &ant_id,
        //    &azimuth,
        //    &elevation
        //);
        //rbody->m_short_frmae_sp.m_z2_distance = distance;                    //Cang与Z2距离
        //rbody->m_short_frmae_sp.m_z2_fangwei = azimuth;                    //CANG终端与QI2终端波束方位指向
        //rbody->m_short_frmae_sp.m_z2_fuyang = elevation;                     //CANG终端与QI2终端波束俯仰指向
        //distance = caculate_distance(overall_fddi_info[0].pos, overall_fddi_info[3].pos);
        //calculate_ante_angle_coord_m(
        //    overall_fddi_info[0].pos.x,
        //    overall_fddi_info[0].pos.y,
        //    overall_fddi_info[0].pos.z,
        //    overall_fddi_info[0].q.q0,
        //    overall_fddi_info[0].q.q1,
        //    overall_fddi_info[0].q.q2,
        //    overall_fddi_info[0].q.q3,
        //    0,
        //    overall_fddi_info[3].pos.x,
        //    overall_fddi_info[3].pos.y,
        //    overall_fddi_info[3].pos.z,
        //    &ant_id,
        //    &azimuth,
        //    &elevation
        //);
        //rbody->m_short_frmae_sp.m_z3_distance = distance;                    //Cang与Z3距离
        //rbody->m_short_frmae_sp.m_z3_fangwei = azimuth;                    //CANG终端与QI3终端波束方位指向
        //rbody->m_short_frmae_sp.m_z3_fuyang = elevation;                     //CANG终端与QI3终端波束俯仰指向
        //distance = caculate_distance(overall_fddi_info[0].pos, overall_fddi_info[4].pos);
        //calculate_ante_angle_coord_m(
        //    overall_fddi_info[0].pos.x,
        //    overall_fddi_info[0].pos.y,
        //    overall_fddi_info[0].pos.z,
        //    overall_fddi_info[0].q.q0,
        //    overall_fddi_info[0].q.q1,
        //    overall_fddi_info[0].q.q2,
        //    overall_fddi_info[0].q.q3,
        //    0,
        //    overall_fddi_info[4].pos.x,
        //    overall_fddi_info[4].pos.y,
        //    overall_fddi_info[4].pos.z,
        //    &ant_id,
        //    &azimuth,
        //    &elevation
        //);
        //rbody->m_short_frmae_sp.m_z4_distance = distance;                    //Cang与Z4距离
        //rbody->m_short_frmae_sp.m_z4_fangwei = azimuth;                    //CANG终端与QI4终端波束方位指向
        //rbody->m_short_frmae_sp.m_z4_fuyang = elevation;                     //CANG终端与QI4终端波束俯仰指向
        //rbody->m_short_frmae_sp.array_status=display_data.array_status;                // 阵面工作状态
        //rbody->m_short_frmae_sp.instruction_crc_error_count= display_data.instruction_crc_error_count; // 指令CRC错误计数
        //rbody->m_short_frmae_sp.address_error_count=display_data.address_error_count;         // 地址码错误计数
        //rbody->m_short_frmae_sp.air_packet_loss_count=display_data.air_packet_loss_count;       // 空口接收丢包计数


		//rbody->m_short_frmae_sp.node_id = 0x10;
		//rbody->m_short_frmae_sp.link_status = info.znode_connect_flag[0]| (info.znode_connect_flag[1]<<1 )| (info.znode_connect_flag[2] << 2) | (info.znode_connect_flag[3] << 3);//目前只有z1，为0001
		//rbody->m_short_frmae_sp.m_proc_flight_control_data_rx_count = info.m_proc_flight_control_data_rx_count;
		//rbody->m_short_frmae_sp.m_proc_flight_control_data_tx_count = info.m_proc_flight_control_data_tx_count;
		//rbody->m_short_frmae_sp.m_z1_air_interface_data_rx_count = g_node_progrm[1].air_interface_data_rx_count;
		//rbody->m_short_frmae_sp.m_z1_air_interface_data_tx_count = g_node_progrm[1].air_interface_data_tx_count;
        memset((uint8_t*)&rbody->m_short_frmae_sp, 0, M_SHORT_FRAME_SP_LEN);
        //rbody->m_short_frmae_sp.time_ljqi = my_get_time();
        //rbody->m_short_frmae_sp.node_id = id_table[MY_INDEX];
        //rbody->m_short_frmae_sp.m_proc_flight_control_data_tx_count = info.m_proc_flight_control_data_tx_count;
        //rbody->m_short_frmae_sp.m_proc_flight_control_data_rx_count = info.m_proc_flight_control_data_rx_count;

        //尾部加载
        memset((uint8_t*)&rbody->m_short_frmae_sp + M_SHORT_FRAME_SP_LEN + 2, 0x7e, 4);
        //crc加载
        *(uint16_t*)((uint8_t*)&rbody->m_short_frmae_sp + M_SHORT_FRAME_SP_LEN) = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + M_SHORT_FRAME_SP_LEN);

        send_to_rs(RS_M_SHORT_FRAME_SP_LEN, 0, res);
    }
    else //Z
    { 
		info.z_proc_flight_control_data_rx_count++;
        uint16_t get_crc = *(uint16_t*)((uint8_t*)&body->z_short_frmae + Z_SHORT_FRAME_LEN);
        if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN + Z_SHORT_FRAME_LEN, get_crc) == 1)
        {
            //printf("z short frame crc fail\n");
            return;
        }
        //内容
#if 1
		memset((uint8_t*)&rbody->z_short_frmae_sp, 0, 77);

        rbody->z_short_frmae_sp.time_element_number = display_data.time_element_number;
        rbody->z_short_frmae_sp.time_frame_number = display_data.time_frame_number;
        rbody->z_short_frmae_sp.micro_time_slot_number = display_data.micro_time_slot_number;
        rbody->z_short_frmae_sp.node_id = id_table[MY_INDEX];
        rbody->z_short_frmae_sp.link_status = info.znode_connect_flag[MY_INDEX - 1]; //暂不加zz之间通信状态，只有当前节点是否建链标志 

        distance = caculate_distance(overall_fddi_info[0].pos, overall_fddi_info[MY_INDEX].pos);
        calculate_ante_angle_coord_z(
            overall_fddi_info[MY_INDEX].pos.x,
            overall_fddi_info[MY_INDEX].pos.y,
            overall_fddi_info[MY_INDEX].pos.z,
            overall_fddi_info[MY_INDEX].q.q0,
            overall_fddi_info[MY_INDEX].q.q1,
            overall_fddi_info[MY_INDEX].q.q2,
            overall_fddi_info[MY_INDEX].q.q3,
            MY_INDEX,
            overall_fddi_info[0].pos.x,
            overall_fddi_info[0].pos.y,
            overall_fddi_info[0].pos.z,
            &ant_id,
            &azimuth,
            &elevation
        );
        rbody->z_short_frmae_sp.z1_m_distance = distance;
        rbody->z_short_frmae_sp.z1_m_azimuth = azimuth;
        rbody->z_short_frmae_sp.z1_m_elevation = elevation;

        rbody->z_short_frmae_sp.comm_status_mode = display_data.comm_status_mode;
        rbody->z_short_frmae_sp.z_proc_flight_control_data_tx_count = info.z_proc_flight_control_data_tx_count;
        rbody->z_short_frmae_sp.z_proc_flight_control_data_rx_count = info.z_proc_flight_control_data_rx_count;
        rbody->z_short_frmae_sp.z_proc_flight_control_data_tx_timestamp = display_data.tx_timestamp;
        rbody->z_short_frmae_sp.z_proc_flight_control_data_rx_timestamp = display_data.rx_timestamp;
        rbody->z_short_frmae_sp.z1_m_air_interface_data_tx_count = g_node_progrm[0].air_interface_data_tx_count;
        rbody->z_short_frmae_sp.z1_m_air_interface_data_rx_count = g_node_progrm[0].air_interface_data_rx_count;
        rbody->z_short_frmae_sp.channel_coding_frame_count = display_data.channel_coding_frame_count;
        rbody->z_short_frmae_sp.channel_decoding_frame_count = display_data.channel_decoding_frame_count;
        rbody->z_short_frmae_sp.modulation_frame_count = display_data.modulation_frame_count;
        rbody->z_short_frmae_sp.demodulation_frame_count = display_data.demodulation_frame_count;
        rbody->z_short_frmae_sp.instruction_parsing_frame_count = display_data.instruction_parsing_frame_count;
        rbody->z_short_frmae_sp.array_status = display_data.array_status;
        rbody->z_short_frmae_sp.instruction_crc_error_count = display_data.instruction_crc_error_count;
        rbody->z_short_frmae_sp.address_error_count = display_data.address_error_count;
        rbody->z_short_frmae_sp.air_packet_loss_count = display_data.air_packet_loss_count;
        rbody->z_short_frmae_sp.recv_error_count = display_data.recv_error_count;
        rbody->z_short_frmae_sp.send_error_count = display_data.send_error_count;           //发送数据失败计数
        rbody->z_short_frmae_sp.antenna_selection_state = display_data.antenna_selection_state;    //天线选择状态
        rbody->z_short_frmae_sp.antenna_txrx_state = display_data.antenna_txrx_state;         //天线收发状态
        rbody->z_short_frmae_sp.beam_width = display_data.beam_width;                 //波束宽度
#endif
		//rbody->z_short_frmae_sp.node_id = 0x11;
		//rbody->z_short_frmae_sp.link_status = info.znode_connect_flag[MY_INDEX-1]; //暂不加zz之间通信状态，只有当前节点是否建链标志 
		//rbody->z_short_frmae_sp.z_proc_flight_control_data_rx_count = info.z_proc_flight_control_data_rx_count;
		//rbody->z_short_frmae_sp.z_proc_flight_control_data_tx_count = info.z_proc_flight_control_data_tx_count;
		//rbody->z_short_frmae_sp.z1_m_air_interface_data_rx_count = g_node_progrm[0].air_interface_data_rx_count;
		//rbody->z_short_frmae_sp.z1_m_air_interface_data_tx_count = g_node_progrm[0].air_interface_data_tx_count;


        //尾部加载
        memset((uint8_t*)&rbody->z_short_frmae_sp + Z_SHORT_FRAME_SP_LEN + 2, 0x7e, 4);
        //crc加载
        *(uint16_t*)((uint8_t*)&rbody->z_short_frmae_sp + Z_SHORT_FRAME_SP_LEN) = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + Z_SHORT_FRAME_SP_LEN);


        send_to_rs(RS_Z_SHORT_FRAME_SP_LEN, 0, res);
    }
}

//长帧回复
void rs_LongFrame_proc(char* data,int len)
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* head = (rs_head_t*)data;
    rs_body_t* body = (rs_body_t*)(data + RS_HEAD_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);
    int ant_id;
    float distance;
    float azimuth;
    float elevation;
    //头部装载
    head_load(data, res);
    //printf("%x\n", body->long_frame_gui.typec);
    if (body->long_frame_gui.typec == 0x33) {
        //crc校验
        if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN + LONG_FRAME_GUI_LEN, body->long_frame_gui.tail.crc) == 1)
        {
            //printf("long frame gui crc fail\n");
            return;
        }
#ifdef PRINT
        p_log_string(0, "recv type=RS_LONG_FRAME_GUI\n");
#endif
        //get(&common_data[Z_GUI_RECV], &rbody->long_frame_sp_gui.typec, Z_GUI_RECV_LEN, 0);
        //rbody->long_frame_sp_gui.typec = 0x33;
        //
        //rbody->long_frame_sp_gui.z_short_frame_sp.node_id = MY_INDEX;
        //rbody->long_frame_sp_gui.framec = 0x20;


        //rbody->long_frame_sp_gui.z_short_frame_sp.time_element_number = display_data.time_element_number;
        //rbody->long_frame_sp_gui.z_short_frame_sp.time_frame_number = display_data.time_frame_number;
        //rbody->long_frame_sp_gui.z_short_frame_sp.micro_time_slot_number = display_data.micro_time_slot_number++;
        //rbody->long_frame_sp_gui.z_short_frame_sp.node_id = id_table[MY_INDEX];
        //rbody->long_frame_sp_gui.z_short_frame_sp.link_status = info.znode_connect_flag[MY_INDEX - 1]; //暂不加zz之间通信状态，只有当前节点是否建链标志 

        //distance = caculate_distance(overall_fddi_info[0].pos, overall_fddi_info[MY_INDEX].pos);
        ////calculate_ante_angle_coord_z(
        ////    overall_fddi_info[MY_INDEX].pos.x,
        ////    overall_fddi_info[MY_INDEX].pos.y,
        ////    overall_fddi_info[MY_INDEX].pos.z,
        ////    overall_fddi_info[MY_INDEX].q.q0,
        ////    overall_fddi_info[MY_INDEX].q.q1,
        ////    overall_fddi_info[MY_INDEX].q.q2,
        ////    overall_fddi_info[MY_INDEX].q.q3,
        ////    MY_INDEX,
        ////    overall_fddi_info[0].pos.x,
        ////    overall_fddi_info[0].pos.y,
        ////    overall_fddi_info[0].pos.z,
        ////    &ant_id,
        ////    &azimuth,
        ////    &elevation
        ////);
        //rbody->long_frame_sp_gui.z_short_frame_sp.z1_m_distance = distance;
        //rbody->long_frame_sp_gui.z_short_frame_sp.z1_m_azimuth = azimuth;
        //rbody->long_frame_sp_gui.z_short_frame_sp.z1_m_elevation = elevation;

        //rbody->long_frame_sp_gui.z_short_frame_sp.comm_status_mode = display_data.comm_status_mode;
        //rbody->long_frame_sp_gui.z_short_frame_sp.z_proc_flight_control_data_tx_count = info.z_proc_flight_control_data_tx_count;
        //rbody->long_frame_sp_gui.z_short_frame_sp.z_proc_flight_control_data_rx_count = info.z_proc_flight_control_data_rx_count;
        //rbody->long_frame_sp_gui.z_short_frame_sp.z_proc_flight_control_data_tx_timestamp = display_data.tx_timestamp;
        //rbody->long_frame_sp_gui.z_short_frame_sp.z_proc_flight_control_data_rx_timestamp = display_data.rx_timestamp;
        //rbody->long_frame_sp_gui.z_short_frame_sp.z1_m_air_interface_data_tx_count = g_node_progrm[0].air_interface_data_tx_count;
        //rbody->long_frame_sp_gui.z_short_frame_sp.z1_m_air_interface_data_rx_count = g_node_progrm[0].air_interface_data_rx_count;
        //rbody->long_frame_sp_gui.z_short_frame_sp.channel_coding_frame_count = display_data.channel_coding_frame_count;
        //rbody->long_frame_sp_gui.z_short_frame_sp.channel_decoding_frame_count = display_data.channel_decoding_frame_count;
        //rbody->long_frame_sp_gui.z_short_frame_sp.modulation_frame_count = display_data.modulation_frame_count;
        //rbody->long_frame_sp_gui.z_short_frame_sp.demodulation_frame_count = display_data.demodulation_frame_count;
        //rbody->long_frame_sp_gui.z_short_frame_sp.instruction_parsing_frame_count = display_data.instruction_parsing_frame_count;
        //rbody->long_frame_sp_gui.z_short_frame_sp.array_status = display_data.array_status;
        //rbody->long_frame_sp_gui.z_short_frame_sp.instruction_crc_error_count = display_data.instruction_crc_error_count;
        //rbody->long_frame_sp_gui.z_short_frame_sp.address_error_count = display_data.address_error_count;
        //rbody->long_frame_sp_gui.z_short_frame_sp.air_packet_loss_count = display_data.air_packet_loss_count;
        //rbody->long_frame_sp_gui.z_short_frame_sp.recv_error_count = display_data.recv_error_count;
        //rbody->long_frame_sp_gui.z_short_frame_sp.send_error_count = display_data.send_error_count;           //发送数据失败计数
        //rbody->long_frame_sp_gui.z_short_frame_sp.antenna_selection_state = display_data.antenna_selection_state;    //天线选择状态
        //rbody->long_frame_sp_gui.z_short_frame_sp.antenna_txrx_state = display_data.antenna_txrx_state;         //天线收发状态
        //rbody->long_frame_sp_gui.z_short_frame_sp.beam_width = display_data.beam_width;                 //波束宽度

        //rbody->long_frame_sp_gui.x2y=0xa;

        //if (return_flag == 0)
        //{
        //    //尾部装载
        //    memcpy(&rbody->long_frame_sp_gui.tail, &body->long_frame_gui.tail, RS_TAIL_LEN);
        //    //crc加载
        //    rbody->long_frame_sp_gui.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + LONG_FRAME_SP_GUI_LEN);
        //    send_to_rs(RS_LONG_FRAME_SP_GUI_LEN, 0, res);
        //}

        
        put(&common_data[Z_GUI_SEND], &body->long_frame_gui.typec, Z_GUI_SEND_LEN,0);
        send_flag = Z_GUI_SEND_LEN;

    }
    else if(body->long_frame_tom.typec == 0x66){
        //crc校验
        if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN + LONG_FRAME_TOM_LEN, body->long_frame_tom.tail.crc) == 1)
        {
            //printf("long frame tom crc fail\n");
            return;
        }
#ifdef PRINT
        p_log_string(0, "recv type=RS_LONG_FRAME_TOM\n");
#endif

        //get(&common_data[Z_TOM_RECV], &rbody->long_frame_sp_tom.typec, Z_TOM_RECV_LEN, 0);
        //rbody->long_frame_sp_tom.typec = 0x66;

        //if (return_flag == 0)
        //{
        //    //尾部装载
        //    memcpy(&rbody->long_frame_sp_tom.tail, &body->long_frame_tom.tail, RS_TAIL_LEN);
        //    //crc加载
        //    rbody->long_frame_sp_tom.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + LONG_FRAME_SP_TOM_LEN);
        //    send_to_rs(RS_LONG_FRAME_SP_TOM_LEN, 0, res);
        //}


        put(&common_data[Z_TOM_SEND], &body->long_frame_tom.typec, Z_TOM_SEND_LEN, 0);
        send_flag = Z_TOM_SEND_LEN;
    }
    //printf("%d\n", return_flag);
    if (return_flag == Z_GUI_RECV_LEN)
    {
        //printf("int&&&&&&&&&&&&&&&&&&&&\n");
        get(&common_data[Z_GUI_RECV], &rbody->long_frame_sp_gui.typec, Z_GUI_RECV_LEN, 0);
        rbody->long_frame_sp_gui.typec = 0x33;

        rbody->long_frame_sp_gui.z_short_frame_sp.node_id = MY_INDEX;
        rbody->long_frame_sp_gui.framec = frameseq++;


        rbody->long_frame_sp_gui.z_short_frame_sp.time_element_number = display_data.time_element_number;
        rbody->long_frame_sp_gui.z_short_frame_sp.time_frame_number = display_data.time_frame_number;
        rbody->long_frame_sp_gui.z_short_frame_sp.micro_time_slot_number = display_data.micro_time_slot_number++;
        rbody->long_frame_sp_gui.z_short_frame_sp.node_id = id_table[MY_INDEX];
        rbody->long_frame_sp_gui.z_short_frame_sp.link_status = info.znode_connect_flag[MY_INDEX - 1]; //暂不加zz之间通信状态，只有当前节点是否建链标志 

        distance = caculate_distance(overall_fddi_info[0].pos, overall_fddi_info[MY_INDEX].pos);
        //calculate_ante_angle_coord_z(
        //    overall_fddi_info[MY_INDEX].pos.x,
        //    overall_fddi_info[MY_INDEX].pos.y,
        //    overall_fddi_info[MY_INDEX].pos.z,
        //    overall_fddi_info[MY_INDEX].q.q0,
        //    overall_fddi_info[MY_INDEX].q.q1,
        //    overall_fddi_info[MY_INDEX].q.q2,
        //    overall_fddi_info[MY_INDEX].q.q3,
        //    MY_INDEX,
        //    overall_fddi_info[0].pos.x,
        //    overall_fddi_info[0].pos.y,
        //    overall_fddi_info[0].pos.z,
        //    &ant_id,
        //    &azimuth,
        //    &elevation
        //);
        rbody->long_frame_sp_gui.z_short_frame_sp.z1_m_distance = distance;
        rbody->long_frame_sp_gui.z_short_frame_sp.z1_m_azimuth = azimuth;
        rbody->long_frame_sp_gui.z_short_frame_sp.z1_m_elevation = elevation;

        rbody->long_frame_sp_gui.z_short_frame_sp.comm_status_mode = display_data.comm_status_mode;
        rbody->long_frame_sp_gui.z_short_frame_sp.z_proc_flight_control_data_tx_count = info.z_proc_flight_control_data_tx_count;
        rbody->long_frame_sp_gui.z_short_frame_sp.z_proc_flight_control_data_rx_count = info.z_proc_flight_control_data_rx_count;
        rbody->long_frame_sp_gui.z_short_frame_sp.z_proc_flight_control_data_tx_timestamp = display_data.tx_timestamp;
        rbody->long_frame_sp_gui.z_short_frame_sp.z_proc_flight_control_data_rx_timestamp = display_data.rx_timestamp;
        rbody->long_frame_sp_gui.z_short_frame_sp.z1_m_air_interface_data_tx_count = g_node_progrm[0].air_interface_data_tx_count;
        rbody->long_frame_sp_gui.z_short_frame_sp.z1_m_air_interface_data_rx_count = g_node_progrm[0].air_interface_data_rx_count;
        rbody->long_frame_sp_gui.z_short_frame_sp.channel_coding_frame_count = display_data.channel_coding_frame_count;
        rbody->long_frame_sp_gui.z_short_frame_sp.channel_decoding_frame_count = display_data.channel_decoding_frame_count;
        rbody->long_frame_sp_gui.z_short_frame_sp.modulation_frame_count = display_data.modulation_frame_count;
        rbody->long_frame_sp_gui.z_short_frame_sp.demodulation_frame_count = display_data.demodulation_frame_count;
        rbody->long_frame_sp_gui.z_short_frame_sp.instruction_parsing_frame_count = display_data.instruction_parsing_frame_count;
        rbody->long_frame_sp_gui.z_short_frame_sp.array_status = display_data.array_status;
        rbody->long_frame_sp_gui.z_short_frame_sp.instruction_crc_error_count = display_data.instruction_crc_error_count;
        rbody->long_frame_sp_gui.z_short_frame_sp.address_error_count = display_data.address_error_count;
        rbody->long_frame_sp_gui.z_short_frame_sp.air_packet_loss_count = display_data.air_packet_loss_count;
        rbody->long_frame_sp_gui.z_short_frame_sp.recv_error_count = display_data.recv_error_count;
        rbody->long_frame_sp_gui.z_short_frame_sp.send_error_count = display_data.send_error_count;           //发送数据失败计数
        rbody->long_frame_sp_gui.z_short_frame_sp.antenna_selection_state = display_data.antenna_selection_state;    //天线选择状态
        rbody->long_frame_sp_gui.z_short_frame_sp.antenna_txrx_state = display_data.antenna_txrx_state;         //天线收发状态
        rbody->long_frame_sp_gui.z_short_frame_sp.beam_width = display_data.beam_width;                 //波束宽度

        rbody->long_frame_sp_gui.x2y = 0xa;

        //尾部装载
        //memcpy(&rbody->long_frame_sp_gui.tail, &body->long_frame_gui.tail, RS_TAIL_LEN);
        memset(&rbody->long_frame_sp_gui.tail.flag, 0x7e, 4);
        //crc加载
        rbody->long_frame_sp_gui.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + LONG_FRAME_SP_GUI_LEN);
        send_to_rs(RS_LONG_FRAME_SP_GUI_LEN, 0, res);

        //return_flag = 0;
    }
    else if (return_flag == Z_TOM_RECV_LEN)
    {
        rbody->long_frame_sp_tom.z_short_frame_sp.time_element_number = display_data.time_element_number;
        rbody->long_frame_sp_tom.z_short_frame_sp.time_frame_number = display_data.time_frame_number;
        rbody->long_frame_sp_tom.z_short_frame_sp.micro_time_slot_number = display_data.micro_time_slot_number++;
        rbody->long_frame_sp_tom.z_short_frame_sp.node_id = id_table[MY_INDEX];
        rbody->long_frame_sp_tom.z_short_frame_sp.link_status = info.znode_connect_flag[MY_INDEX - 1]; //暂不加zz之间通信状态，只有当前节点是否建链标志 
        get(&common_data[Z_TOM_RECV], &rbody->long_frame_sp_tom.typec, Z_TOM_RECV_LEN, 0);
        rbody->long_frame_sp_tom.typec = 0x66;
        rbody->long_frame_sp_tom.framec = frameseq++;
        //尾部装载
        //memcpy(&rbody->long_frame_sp_tom.tail, &body->long_frame_tom.tail, RS_TAIL_LEN);
        memset(&rbody->long_frame_sp_tom.tail.flag, 0x7e, 4);
        //crc加载
        rbody->long_frame_sp_tom.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + LONG_FRAME_SP_TOM_LEN);
        send_to_rs(RS_LONG_FRAME_SP_TOM_LEN, 0, res);

        //return_flag = 0;
    }
    else if (return_flag == 0 && body->long_frame_gui.typec == 0x33)
    {
        rbody->long_frame_sp_gui.z_short_frame_sp.micro_time_slot_number = display_data.micro_time_slot_number++;
        rbody->long_frame_sp_gui.typec = 0x33;
        //尾部装载
        memset(&rbody->long_frame_sp_gui.tail.flag, 0x7e, 4);
        //crc加载
        rbody->long_frame_sp_gui.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + LONG_FRAME_SP_GUI_LEN);
        send_to_rs(RS_LONG_FRAME_SP_GUI_LEN, 0, res);
    }
    else if (return_flag == 0 && body->long_frame_tom.typec == 0x66)
    {
        rbody->long_frame_sp_gui.z_short_frame_sp.micro_time_slot_number = display_data.micro_time_slot_number++;
        rbody->long_frame_sp_tom.typec = 0x66;
        //尾部装载
        memset(&rbody->long_frame_sp_tom.tail.flag, 0x7e, 4);
        //crc加载
        rbody->long_frame_sp_tom.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + LONG_FRAME_SP_TOM_LEN);
        send_to_rs(RS_LONG_FRAME_SP_TOM_LEN, 0, res);
    }

}

void head_load(char* data,char* res)
{
    rs_head_t* head = (rs_head_t*)data;
    rs_head_t* rhead = (rs_head_t*)res;
    memcpy(res, data, RS_HEAD_LEN);
    rhead->address_a = head->address_b;
    rhead->address_b = head->address_a;
    //printf("%x %x\n", head->address_a, head->address_b);
    //printf("%x %x\n", rhead->address_a, rhead->address_b);
}

void head_load_b(char* res)
{
    rs_head_t* rhead = (rs_head_t*)res;
    memset(res, 0x7e, 4);
    rhead->address_a = MY_INDEX == 0 ? 0x50 : 0x52;
    rhead->address_b = MY_INDEX == 0 ? 0x56 : 0x78;
}

uint8_t crc_check(char* start_address, int len, uint16_t get_crc)
{
    int i = 0;
    uint16_t crc = CalCRC16_V2(start_address, len);
    //printf("crc check:");
    //for (i = 0; i < len; i++)
    //{
    //    printf("%02x ", (uint8_t)start_address[i]);
    //}
    //printf("\n");
    //printf("crc len:%d recv crc:%x,calculate crc:%x crc ok\n", len, get_crc, crc);
    if (crc != get_crc)
    {
        printf("crc len:%d recv crc:%x,calculate crc:%x crc fail\n", len, get_crc, crc);
        return 1;
    }
    else
    {
        //printf("crc len:%d recv crc:%x,calculate crc:%x crc ok\n", len, get_crc, crc);
    }
    return 0;
}


//**********************************//
void rs_M2ZGui_proc(char* data)
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* head = (rs_head_t*)data;
    rs_body_t* body = (rs_body_t*)(data + RS_HEAD_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);
    //crc
    uint16_t get_crc = body->m2z_gui_frame.tail.crc;
    if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN + M2Z_GUI_FRAME_LEN, get_crc) == 1)
    {
        //printf("M2ZGui crc fail\n");
        return;
    }
    ////头部
    //head_load(data, res);
    //rhead->type = RS_Z2M_GUI_SP;
    ////内容
    //mget(&common_data[M_GUI_RECV], rbody->m2z_gui_frame_sp.content, 4 * M_GUI_RECV_LEN);

    ////尾部
    //memset(&rbody->m2z_gui_frame_sp.tail.flag, 0x7e, 4);

    ////crc加载
    //rbody->m2z_gui_frame_sp.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + M2Z_GUI_FRAME_SP_LEN);
    //send_to_rs(RS_M2Z_GUI_FRAME_SP_LEN, 0, res);

    //p_log_data(0, RS_M2Z_GUI_FRAME_SP_LEN, res);

    //enqueue(&info.thread_queue[MASTER_THREAD], body->m2z_gui_frame.content, 132);
    mput(&common_data[M_GUI_SEND], body->m2z_gui_frame.content, 4 * M_GUI_SEND_LEN);
    send_flag = M_GUI_SEND_LEN;
    //ppp(&common_data[M_GUI_SEND], M_GUI_SEND_LEN*4, 0);

}

void rs_M2ZTom_proc(char* data)
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* head = (rs_head_t*)data;
    rs_body_t* body = (rs_body_t*)(data + RS_HEAD_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);
    //crc
    uint16_t get_crc = body->m2z_tom_frame.tail.crc;
    if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN + M2Z_TOM_FRAME_LEN, get_crc) == 1)
    {
        //printf("M2ZTom crc fail\n");
        return;
    }
    ////头部
    //head_load(data, res);
    //rhead->type = RS_Z2M_TOM_SP;
    ////内容
    //mget(&common_data[M_TOM_RECV], rbody->m2z_tom_frame_sp.content, 4 * M_TOM_RECV_LEN);

    ////尾部
    //memset(&rbody->m2z_tom_frame_sp.tail.flag, 0x7e, 4);
    ////crc
    //rbody->m2z_tom_frame_sp.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + M2Z_TOM_FRAME_SP_LEN);
    //send_to_rs(RS_M2Z_TOM_FRAME_SP_LEN, 0, res);

    //p_log_data(0, RS_M2Z_TOM_FRAME_SP_LEN, res);

    //body->m2z_tom_frame.content[1] = 0xAA;
    //body->m2z_tom_frame.content[2] = 0xBB;
    //enqueue(&info.thread_queue[MASTER_THREAD], body->m2z_tom_frame.content, 1828);
    mput(&common_data[M_TOM_SEND], body->m2z_tom_frame.content, 4 * M_TOM_SEND_LEN);
    send_flag = M_TOM_SEND_LEN;
}

void rs_M2ZPlan_proc(char* data)
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* head = (rs_head_t*)data;
    rs_body_t* body = (rs_body_t*)(data + RS_HEAD_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);
    //crc
    uint16_t get_crc = body->m2z_plan_frame.tail.crc;
    if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN + M2Z_PLAN_FRAME_LEN, get_crc) == 1)
    {
        //printf("M2ZPlan crc fail\n");
        return;
    }
    //头部
    head_load(data, res);
    rhead->type = RS_Z2M_PLAN_SP;
    //内容

    //尾部
    memset(&rbody->m2z_plan_frame_sp.tail.flag, 0x7e, 4);
    //crc
    rbody->m2z_plan_frame_sp.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + M2Z_PLAN_FRAME_SP_LEN);
    send_to_rs(RS_M2Z_PLAN_FRAME_SP_LEN, 0, res);

}



void create_CheckResult_res()
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);

    //head
    head_load_b(res);
    //type
    rhead->type = 0x1010;
    //content
	if (MY_INDEX == 0)
    {
        rbody->m_result1_ack.ack = 0;
        rbody->m_result1_ack.version[0] = 0x02;
        rbody->m_result1_ack.version[1] = 0x41;
        rbody->m_result1_ack.version[2] = 0x01;
        rbody->m_result1_ack.version[3] = 0x02;
        rbody->m_result1_ack.version[4] = 0x41;
        rbody->m_result1_ack.version[5] = 0x01;
        rbody->m_result1_ack.version[6] = 0x02;
        rbody->m_result1_ack.version[7] = 0x41;
        rbody->m_result1_ack.version[8] = 0x01;
       //crc
    rbody->m_result1_ack.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + 2+9);
    //tail
    memset(&rbody->m_result1_ack.tail.flag, 0x7e, 4);
	//send
    send_to_rs(RS_RESULT_LEN+9, 0, res);
    }
    else
    {
        rbody->z_result1_ack.ack = 0;
        rbody->z_result1_ack.version[0] = 0x02;
        rbody->z_result1_ack.version[1] = 0x41;
        rbody->z_result1_ack.version[2] = 0x01;
        rbody->z_result1_ack.version[3] = 0x02;
        rbody->z_result1_ack.version[4] = 0x41;
        rbody->z_result1_ack.version[5] = 0x01;
        //crc
    rbody->z_result1_ack.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + 2+6);
    //tail
    memset(&rbody->z_result1_ack.tail.flag, 0x7e, 4);
	//send
    send_to_rs(RS_RESULT_LEN+6, 0, res);
    }
    
}

void create_ConfigLoad_res()
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);

    //head
    head_load_b(res);
    //type
    rhead->type = 0xCCCC;
    //content
    
    //crc
    rbody->config_load.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + CONFIG_INFO_LEN);
    //tail
    memset(&rbody->config_load.tail.flag, 0x7e, 4);
    //send
    send_to_rs(RS_CONFIG_LEN, 0, res);
}

void create_WorkMode_res()
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);

    //head
    head_load_b(res);
    //type
    rhead->type = 0x7070;
    //content

    //crc
    rbody->work_modes_sp.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + 1);
    //tail
    memset(&rbody->work_modes_sp.tail.flag, 0x7e, 4);
    //send
    send_to_rs(RS_WORK_MODE_SP_LEN, 0, res);
}

void create_LinkResult_res()
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);

    //head
    head_load_b(res);
    //type
    rhead->type = 0x4040;
    //content

    //crc
    rbody->link_result_sp.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + LINK_RESULT_SP_LEN);
    //tail
    memset(&rbody->link_result_sp.tail.flag, 0x7e, 4);
    //send
    send_to_rs(RS_LINK_RESULT_SP_LEN, 0, res);

}

void create_ShortFrame_res()
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);

    //head
    head_load_b(res);
    //type
    rhead->type = 0x3030;

    if (MY_INDEX == 0) //M
    {
        //content

        //尾部加载
        memset((uint8_t*)&rbody->m_short_frmae_sp + M_SHORT_FRAME_SP_LEN + 2, 0x7e, 4);
        //crc加载
        *(uint16_t*)((uint8_t*)&rbody->m_short_frmae_sp + M_SHORT_FRAME_SP_LEN) = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + M_SHORT_FRAME_SP_LEN);

        send_to_rs(RS_M_SHORT_FRAME_SP_LEN, 0, res);
    }
    else //Z
    {
        //内容

        //尾部加载
        memset((uint8_t*)&rbody->z_short_frmae_sp + Z_SHORT_FRAME_SP_LEN + 2, 0x7e, 4);
        //crc加载
        *(uint16_t*)((uint8_t*)&rbody->z_short_frmae_sp + Z_SHORT_FRAME_SP_LEN) = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + Z_SHORT_FRAME_SP_LEN);

        send_to_rs(RS_Z_SHORT_FRAME_SP_LEN, 0, res);
    }
}






int rs485_init()
{
    int i, ret = 0;
    uint64_t path_len = MAX_PATH + 1;
    char** device_base_path;
    device_base_path = (char**)malloc(sizeof(char*) * 4);
    assert(device_base_path);
    for (i = 0; i < 4; i++)
    {
        device_base_path[i] = (char*)malloc(sizeof(char) * path_len);
        assert(device_base_path[i]);
        memset(device_base_path[i], 0, MAX_PATH + 1);
    }

    uint32_t numBoard = get_devices(GUID_DEVINTERFACE_XDMA, device_base_path, path_len);
    if (!numBoard)
    {

        return -1;
    }
    printf("board num:%d\n", numBoard);
    memcpy(path, device_base_path[0], path_len);
    printf("rs485 recv0x10000000\n");
    ret = open_devices(&recv_hdev, GENERIC_READ, path, (const char*)XDMA_FILE_C2H_0);
    if (ret == 0)
    {
        printf("RS485:open recv fail\n");
    }
    ret = open_devices(&send_hdev, GENERIC_WRITE, path, (const char*)XDMA_FILE_H2C_0);
    if (ret == 0)
    {
        printf("RS485:open send fail\n");
    }
    ret = open_devices(&user_hdev, GENERIC_READ | GENERIC_WRITE, path, (const char*)XDMA_FILE_USER);
    if (ret == 0)
    {
        printf("RS485:open user fail\n");
    }
    ret = reset_devices(user_hdev);
    if (ret < 0)
    {
        printf("RS485:reset fail\n");
        return ret;
    }
    //sem_init(&pcie_semaphore, 0, 0);
    pthread_mutex_init(&pcie_mutex, NULL);
}



void server_init()
{
    int ret = 0;

    //创建侦听socket
    lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //设置端口复用
    int opt = 1;
    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) == -1) plog("setsockopt error");

    //绑定本机ip地址、端口号
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    if (MY_INDEX == 0)
    {
        inet_pton(AF_INET, "127.0.0.1", (void*)&addr.sin_addr); //**ip**
        addr.sin_port = htons(6789); //**port**
    }
    else
    {
        inet_pton(AF_INET, "127.0.0.1", (void*)&addr.sin_addr); //**ip**
        addr.sin_port = htons(7789); //**port**
    }
    ret = bind(lfd, (struct sockaddr*)&addr, sizeof addr); //绑定ip和端口

    //开始监听
    listen(lfd, SOMAXCONN);

    info.control_system.addr_len = sizeof(info.control_system.addr);
    rs485_fd = accept(lfd, (struct sockaddr*)&(info.control_system.addr), &(info.control_system.addr_len));//接收连接请求
    printf("rs485 connect success %d\n", rs485_fd);
}



#pragma pack(1)
struct pcie_check_t
{
    uint8_t flag_head[4];
    uint8_t addressa;
    uint8_t addressb;
    uint8_t flaga;
    uint8_t flagb;
    uint16_t crc;
    uint8_t flag_tail[4];
};
#pragma pack()


void rs_communicate_enalbe_proc(char* data)
{

    struct pcie_check_t* recv = (struct pcie_check_t*)data;

    if (crc_check((uint8_t*)recv->flag_head + 4, 4, recv->crc) == 1)
    {
        //printf("long frame gui crc fail\n");
        return;
    }
    //printf("a=%02x b=%02x\n", recv->flaga, recv->flagb);
}



void pcie_check()
{
    int a;
    int i;
    struct pcie_check_t data;
    uint8_t ret[2048];
    struct pcie_check_t* res = (struct pcie_check_t*)ret;
    memset(&data, 0x7e, 4);
    data.addressa = 0xcc;
    data.addressb = 0xdd;
    data.flaga = 0xeb;
    data.flagb = 0x90;
    memset(&data.flag_tail, 0x7e, 4);
    data.crc = CalCRC16_V2((uint8_t*)&data.addressa, 4);
    printf("checking pcie\n");
    send_to_rs(sizeof(struct pcie_check_t), 0, &data);
    Sleep(1000);
    //a = read_device(recv_hdev, 0x10000000, 2048, ret);
    //a = last_packetSize(user_hdev);
    //for (i = 0; i < a; i++)
    //{
    //    printf("%02x ", ret[i]);
    //}
    //printf("\n");
    //if (res->addressa == 0xdd && res->addressb == 0xcc)
    //{
    //    printf("pcie checking success\n");
    //}
    //printf("pcie checking success\n");
}













