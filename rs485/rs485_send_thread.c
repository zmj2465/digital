#include "rs485_send_thread.h"
#include "rs485_recv_thread.h"
#include "data_send_thread.h"
#include <string.h>


void* rs_485_send_thread(void* arg)
{
    pthread_detach(pthread_self());

    char data[MAX_DATA_LEN];
    int len;
    Sleep(3000);

    while (1)
    {
        if (return_flag == M_GUI_RECV_LEN)
        {
            send_m_gui_sp();
            return_flag = 0;
        }
        else if (return_flag == M_TOM_RECV_LEN)
        {
            send_m_tom_sp();
            return_flag = 0;
        }
        ////enqueue(&info.thread_queue[RS485_RECV_THREAD], data, 565);
        Sleep(25);
    }

}

void send_m_gui_sp()
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    //rs_head_t* head = (rs_head_t*)data;
    //rs_body_t* body = (rs_body_t*)(data + RS_HEAD_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);

    //头部
    memset(rhead, 0x7e, 4);
    rhead->address_a = 0x53;
    rhead->address_b = 0x56;
    rhead->type = RS_Z2M_GUI_SP;
    //内容
    mget(&common_data[M_GUI_RECV], rbody->m2z_gui_frame_sp.content, 4 * M_GUI_RECV_LEN);

    //尾部
    memset(&rbody->m2z_gui_frame_sp.tail.flag, 0x7e, 4);

    //crc加载
    rbody->m2z_gui_frame_sp.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + M2Z_GUI_FRAME_SP_LEN);
    send_to_rs(RS_M2Z_GUI_FRAME_SP_LEN, 0, res);
}

void send_m_tom_sp()
{
    char res[RS_MAX_LEN];
    memset(res, 0, RS_MAX_LEN);
    //rs_head_t* head = (rs_head_t*)data;
    //rs_body_t* body = (rs_body_t*)(data + RS_HEAD_LEN);
    rs_head_t* rhead = (rs_head_t*)res;
    rs_body_t* rbody = (rs_body_t*)(res + RS_HEAD_LEN);

    //头部
    memset(rhead, 0x7e, 4);
    rhead->address_a = 0x53;
    rhead->address_b = 0x56;
    rhead->type = RS_Z2M_TOM_SP;
    //内容
    mget(&common_data[M_TOM_RECV], rbody->m2z_tom_frame_sp.content, 4 * M_TOM_RECV_LEN);

    //尾部
    memset(&rbody->m2z_tom_frame_sp.tail.flag, 0x7e, 4);
    //crc
    rbody->m2z_tom_frame_sp.tail.crc = CalCRC16_V2((uint8_t*)rhead->flag + 4, ADD_TYPE_LEN + M2Z_TOM_FRAME_SP_LEN);
    send_to_rs(RS_M2Z_TOM_FRAME_SP_LEN, 0, res);

    //p_log_data(0, RS_M2Z_TOM_FRAME_SP_LEN, res);
}