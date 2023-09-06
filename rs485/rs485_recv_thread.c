#include "rs485_recv_thread.h"
#include "xdmaDLL_public.h"
#include "stddef.h"
#include <string.h>
#include "crc.h"

#define MSGLEN   4
#define FLAG_LEN 1
#define HEAD_LEN 3
#define CC_LEN   100

char path[1024];


static HANDLE recv_hdev;
static HANDLE send_hdev;
static HANDLE user_hdev;

void rs485_init()
{
    int i;
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

        return 0;
    }
    printf("board num:%d\n", numBoard);
    memcpy(path, device_base_path[0], path_len);
}




void* rs_485_recv_thread(void* arg)
{
    pthread_detach(pthread_self());
    int ret = 0;
    int i;

    rs485_init();

    uint8_t data[1024];
    char res[1024];

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
        return;
    }

#if(0)
    i=0;
    uint8_t a[1024];
    memset(a, 0, 1024);

    a[i++] = 0x7E;
    a[i++] = 0x7E;
    a[i++] = 0x7E;
    a[i++] = 0x7E;

    a[i++] = 0x52;
    a[i++] = 0x78;
    a[i++] = 0x10;
    a[i++] = 0x34;
    a[i++] = 0x12;
    a[i++] = 0xC4;
    a[i++] = 0x74;

    a[i++] = 0x7E;
    a[i++] = 0x7E;
    a[i++] = 0x7E;
    a[i++] = 0x7E;

    //memset(a, 0, 1024);
    
    while (1)
    {
        //ret = write_device(send_hdev, 0x10000000, 16, a);
        //printf("%d\n", ret);
        //Sleep(1000);
        //ret = read_device(recv_hdev, 0x10000000, 1024, a);
        //ret=last_packetSize(user_hdev);
        //printf("get len=%d\n", ret);
        //for (i = 0; i < ret; i++)
        //{
        //    printf("%02x ", a[i]);
        //}
        //printf("\n");

        //ret = write_device(send_hdev, 0x10000000, ret, a);
        //printf("send ok:%d\n",ret);
    }
#else
    while (1)
    {
        ret = read_device(recv_hdev, 0x10000000, 1024, data);
        ret = last_packetSize(user_hdev);

        rs_head_t* head = (rs_head_t*)data;
        printf("-----get len=%d type:%x-----\n", ret, head->typea);
        for (i = 0; i < ret; i++)
        {
            printf("%02x ", data[i]);
        }
        printf("\n");

        
        //Z-M
        switch (head->typea)
        {
        case RS_SELF_CHECK:
            rs_SelfCheck_proc(data);
            break;
        case RS_SLEF_CHECK_RESULT:
            rs_SelfCheckResult_proc(data);
            break;
        case RS_CONFIG_LOAD:
            rs_ConfigLoad_proc(data);
            break;
        case RS_SHORT_FRAME:
            rs_ShortFrame_proc(data);
            break;
        case RS_LONG_FRAME:
            rs_LongFrame_proc(data,ret);
            break;
        case RS_START_LINK:
            rs_Link_proc(data);
            break;

        }
        //M
        switch (head->type)
        {
        case RS_M2Z_GUI:
            rs_M2ZGui_proc(data);
            break;
        case RS_M2Z_TOM:
            rs_M2ZTom_proc(data);
            break;
        case RS_M2Z_PLAN:
            rs_M2ZPlan_proc(data);
            break;

        }
    }
#endif
}


void send_to_rs(int len, long offset, char* data)
{
    int ret = 0;
    int i;
    rs_head_t* rhead = (rs_head_t*)data;

    printf("*****send:%d type:%x*****\n", len, rhead->typea);
    for (i = 0; i < len; i++)
    {
        printf("%02x ", (uint8_t)data[i]);
    }
    printf("\n");

    ret = write_device(send_hdev, offset, len, data);
    
    printf("$$$$$send ok$$$$$\n");
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
        printf("self check crc fail\n");
        return;
    }

    //装载头部
    head_load(data, res);

    memcpy(&rbody->body_tail, &body->body_tail, RS_TAIL_LEN);

    rbody->body_tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN);

    send_to_rs(RS_SELF_CHECK_LEN, 0, res);
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
        printf("self check result crc fail\n");
        return;
    }
    head_load(data, res);

    rbody->result1_ack.ack = 0;
    memcpy(&rbody->result1_ack.tail, &body->body_tail, RS_TAIL_LEN);

    rbody->result1_ack.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + 2);
    send_to_rs(RS_RESULT_LEN, 0, res);
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
        printf("config load crc fail\n");
        return;
    }

    head_load(data, res);

    memcpy(&rbody->config_load, &body->config_load, CONFIG_INFO_LEN + RS_TAIL_LEN);

    rbody->config_load.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + CONFIG_INFO_LEN);
    send_to_rs(RS_CONFIG_LEN, 0, res);

    //MY_INDEX = body->config_load.node_id;
    //FD_NUM = body->config_load.node_num;

}

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
        printf("link start crc fail\n");
        return;
    }
    //头部
    head_load(data, res);
    //内容
    //尾部

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
    //crc校验
    uint16_t get_crc = *(uint16_t*)((char*)&body->short_frmae + SHORT_FRAME_LEN);
    if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN + SHORT_FRAME_LEN, get_crc) == 1)
    {
        printf("short frame crc fail\n");
        return;
    }
    //头部加载
    head_load(data, res);
    //内容加载
    
    //尾部加载
    memcpy((char*)&rbody->short_frmae_sp + SHORT_FRAME_SP_LEN, (char*)&body->short_frmae + SHORT_FRAME_LEN, RS_TAIL_LEN);
    //crc加载
    *(uint16_t*)((char*)&rbody->short_frmae_sp + SHORT_FRAME_SP_LEN) = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + SHORT_FRAME_SP_LEN);
    send_to_rs(RS_SHORT_FRAME_SP_LEN, 0, res);


    //处理
    memcpy(&overall_fddi_info[MY_INDEX], (uint8_t*)&head->flag + 4 + ADD_TYPE_LEN, sizeof(fddi_info_t));
    printf("x:%f y:%f z:%f\n", overall_fddi_info[MY_INDEX].pos.x,
                               overall_fddi_info[MY_INDEX].pos.x,
                               overall_fddi_info[MY_INDEX].pos.z);
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
    //头部装载
    head_load(data, res);
     
    if (body->long_frame_gui.typec == 0x33) {
        //crc校验
        if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN + LONG_FRAME_GUI_LEN, body->long_frame_gui.tail.crc) == 1)
        {
            printf("long frame gui crc fail\n");
            return;
        }

        rbody->long_frame_sp_gui.typec = 0x33;

        //尾部装载
        memcpy(&rbody->long_frame_sp_gui.tail, &body->long_frame_gui.tail, RS_TAIL_LEN);
        //crc加载
        rbody->long_frame_sp_gui.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + LONG_FRAME_SP_GUI_LEN);
        send_to_rs(RS_LONG_FRAME_SP_GUI_LEN, 0, res);
    }
    else if(body->long_frame_tom.typec == 0x66){
        //crc校验
        if (crc_check((uint8_t*)head->flag + 4, ADD_TYPE_LEN + LONG_FRAME_TOM_LEN, body->long_frame_tom.tail.crc) == 1)
        {
            printf("long frame tom crc fail\n");
            return;
        }

        rbody->long_frame_sp_gui.typec = 0x66;
        //尾部装载
        memcpy(&rbody->long_frame_sp_tom.tail, &body->long_frame_tom.tail, RS_TAIL_LEN);
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
    printf("crc len:%d get crc:%x,crc:%x\n", len, get_crc, crc);
    if (crc != get_crc)
    {
        return 1;
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
        printf("M2ZGui crc fail\n");
        return;
    }
    //头部
    head_load(data, res);
    rhead->type = RS_Z2M_GUI_SP;
    //内容
     
    //尾部
    memset(&rbody->m2z_gui_frame_sp.tail.flag, 0x7e, 4);

    //crc加载
    rbody->m2z_gui_frame_sp.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + M2Z_GUI_FRAME_SP_LEN);
    send_to_rs(RS_M2Z_GUI_FRAME_SP_LEN, 0, res);
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
        printf("M2ZTom crc fail\n");
        return;
    }
    //头部
    head_load(data, res);
    rhead->type = RS_Z2M_TOM_SP;
    //内容

    //尾部
    memset(&rbody->m2z_tom_frame_sp.tail.flag, 0x7e, 4);
    //crc
    rbody->m2z_tom_frame_sp.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + M2Z_TOM_FRAME_SP_LEN);
    send_to_rs(RS_M2Z_TOM_FRAME_SP_LEN, 0, res);
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
        printf("M2ZPlan crc fail\n");
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













