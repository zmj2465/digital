#include <stdio.h>
#include <pthread.h>
#include "rs485_send_thread.h"
#include "rs485_recv_thread.h"
#include "fddi_thread.h"
#include "schedule_thread.h"
#include "control_recv_thread.h"
#include "display_send_thread.h"
#include "master_thread.h"
#include "link_control_thread.h"
#include "data_recv_thread.h"
#include "data_send_thread.h"
#include "queue.h"

#include "compatible.h"
#include <semaphore.h>
#include <time.h>
#include <signal.h>
#include "common.h"
#include "protocol.h"

sem_t semaphore;
pthread_mutex_t lock;

static int temp = 0;


int main()
{
    int ret;

    /*���ӿ��ʼ��*/
    wsa_init();

    /*��Ϣ���г�ʼ��*/
    queue_init();

    /*���ý������ȼ�*/
    set_process_priority();

    /*ip��Ϣ����*/
    load_ip_config();

    /*���ݴ洢��ʼ��*/
    data_store_init();

    /*�ź�����ʼ��*/   
    sem_init(&info.send_semaphore, 0, 0);
    sem_init(&info.thread_create_semaphore, 0, 0);


    /*�̳߳�ʼ��*/
    ret = pthread_create(&info.rs_485_recv_thread_id, NULL, rs_485_recv_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.rs_485_send_thread_id, NULL, rs_485_send_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.fddi_thread_id, NULL, fddi_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.schedule_thread_id, NULL, schedule_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.control_recv_thread_id, NULL, control_recv_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.display_send_thread_id, NULL, display_send_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.master_thread_id, NULL, master_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.link_control_thread_id, NULL, link_control_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    sem_wait(&info.thread_create_semaphore);

    /**/
    ret = pthread_create(&info.data_send_thread_id, NULL, data_recv_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /**/
    ret = pthread_create(&info.data_recv_thread_id, NULL, data_send_thread, NULL);
    if (ret != 0)
    {
        printf("error\n");
    }

    /*���ó�ʼ״̬*/
    fsm_do(EVENT_INIT);

    while (1)
    {
        Sleep(200000);
    }
}


void load_self_config()
{

}

void load_ip_config()
{
    FILE* file;
    int i;
    //char ip[50];
    char line[100];
    // ���ļ�
    file = fopen(OVERALL_FILE, "r");
    if (file == NULL) {
        printf("�޷����ļ���\n");
        return 1;
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        // ȥ����β�Ļ��з�
        line[strcspn(line, END_FLAG)] = '\0';
        // ����Ƿ�Ϊ"[IP]"�У�����ǣ�������
        if (strcmp(line, "[IP]") == 0) {
            while (fgets(line, sizeof(line), file) != NULL)
            {
                line[strcspn(line, END_FLAG)] = '\0';
                if (strcmp(line, "[END]")==0) break;
                // �洢IP��ַ������
                strncpy(FD[FD_NUM].ip, line, IP_LEN);
                FD_NUM++;
            }
            break;
        }
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        // ȥ����β�Ļ��з�
        line[strcspn(line, END_FLAG)] = '\0';
        // ����Ƿ�Ϊ"[PORT]"��
        if (strcmp(line, "[PORT]") == 0) {
            // ��ȡ��һ�еĶ˿ں�
            fgets(line, sizeof(line), file);
            line[strcspn(line, END_FLAG)] = '\0';
            sscanf(line, "communication_port %d", &info.communication_port);
            fgets(line, sizeof(line), file);
            sscanf(line, "fddi_port %d", &info.fddi_port);
            fgets(line, sizeof(line), file);
            sscanf(line, "control_port %d", &info.control_port);
            fgets(line, sizeof(line), file);
            sscanf(line, "display_port %d", &info.display_port);
        }
    }

    // �ر��ļ�
    fclose(file);

    file = fopen(PRIVATE_FILE, "r");
    if (file == NULL) {
        printf("�޷����ļ���\n");
        return 1;
    }

    fgets(line, sizeof(line), file);
    sscanf(line, "%s", info.ip);

    fgets(line, sizeof(line), file);
    sscanf(line, "%s", info.fddi_ip);

    fgets(line, sizeof(line), file);
    sscanf(line, "%s", info.control_ip);

    fgets(line, sizeof(line), file);
    sscanf(line, "%s", info.display_ip);

    MY_INDEX = -1;
    for (i = 0; i < info.simulated_link_num; i++)
    {
        if (strcmp(info.ip, FD[i].ip) == 0)
        {
            MY_INDEX = i;
            break;
        }
    }

    fclose(file);

    if (MY_INDEX == -1)
    {
        printf("config error\n");
        while (1);
    }

    // ��ӡ��ȡ��ֵ
    for (i = 0; i < info.simulated_link_num; i++)
    {
        printf("IP��ַ: %s\n", FD[i].ip);
    }
    printf("communication_port: %d %s\n", info.communication_port,info.ip);
    printf("fddi_port: %d %s\n", info.fddi_port, info.fddi_ip);
    printf("control_port: %d %s\n", info.control_port, info.control_ip);
    printf("display_port: %d %s\n", info.display_port, info.display_ip);
    printf("MY_INDEX=%d\n", MY_INDEX);
}

void load_simulation_config()
{

}

void data_store_init()
{
    // ���ļ�
    HANDLE hFile = CreateFile(OUTPUT_FILE_NAME, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        printf("Failed to open file\n");
        return 1;
    }

    // �����ļ���С
    SetFilePointer(hFile, STORE_SIZE, NULL, FILE_BEGIN);
    SetEndOfFile(hFile);

    // �����ڴ�ӳ��
    HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, STORE_SIZE, NULL);
    if (hMapping == NULL) {
        printf("Failed to create file mapping\n");
        CloseHandle(hFile);
        return 1;
    }

    // ӳ�䵽���̵��ڴ�ռ�
    info.lpSharedMem = MapViewOfFile(hMapping, FILE_MAP_WRITE, 0, 0, STORE_SIZE);
    if (info.lpSharedMem == NULL) {
        printf("Failed to map view of file\n");
        CloseHandle(hMapping);
        CloseHandle(hFile);
        return 1;
    }
}


/*
    * ��������         GetIniKeyString
    * ��ڲ�����        title
    *                      �����ļ���һ�����ݵı�ʶ
    *                  key
    *                      ����������Ҫ������ֵ�ı�ʶ
    *                  filename
    *                      Ҫ��ȡ���ļ�·��
    * ����ֵ��         �ҵ���Ҫ���ֵ�򷵻���ȷ��� 0
    *                  ���򷵻�-1
    */
int GetIniKeyString(char* title, char* key, char* filename, char* buf)
{

    FILE* fp;
    int  flag = 0;
    char sTitle[64], * wTmp;
    char sLine[1024];
    sprintf(sTitle, "[%s]", title);

    if (NULL == (fp = fopen(filename, "r"))) {
        perror("fopen");
        return -1;
    }
    while (NULL != fgets(sLine, 1024, fp)) {
        // ����ע����  
        if (0 == strncmp("//", sLine, 2)) continue;
        if ('#' == sLine[0])              continue;
        wTmp = strchr(sLine, '=');
        if ((NULL != wTmp) && (1 == flag)) {
            if (0 == strncmp(key, sLine, strlen(key))) { // �������ļ���ȡ��Ϊ׼  
                sLine[strlen(sLine) - 1] = '\0';
                fclose(fp);
                while (*(wTmp + 1) == ' ') {
                    wTmp++;
                }
                strcpy(buf, wTmp + 1);
                return 0;
            }
        }
        else {
            if (0 == strncmp(sTitle, sLine, strlen(sTitle))) { // �������ļ���ȡ��Ϊ׼  
                flag = 1; // �ҵ�����λ��  
            }
        }
    }
    fclose(fp);
    return -1;
}

int load_config(char* filename)
{
    char id[20];
    char buff[100][20];
    int ret = 0;
    int i = 0;
    FILE* fp;
    if (NULL == (fp = fopen(filename, "r"))) {
        perror("fopen");
        return -1;
    }
    fgets(id, 20, fp);
    int len = strlen(id);
    id[len - 1] = 0;

    GetIniKeyString(id, "ip", filename, &buff[++i]);
    printf("%d %s\n", ret, buff[i]);
    memcpy(&info.ip, &buff[i], sizeof(buff[i]));

    ret = GetIniKeyString(id, "port", filename, &buff[++i]);
    printf("%d %s\n", ret, buff[i]);


    ret = GetIniKeyString(id, "fddi_ip", filename, &buff[++i]);
    printf("%d %s\n", ret, buff[i]);
    memcpy(&info.fddi_ip, &buff[i], sizeof(buff[i]));

    ret = GetIniKeyString(id, "fddi_port", filename, &buff[++i]);
    printf("%d %s\n", ret, buff[i]);

    //ret = GetIniKeyString(id, "communication_ip", filename, &buff[++i]);
    //printf("%d %s\n", ret, buff[i]);
    //memcpy(&info, &buff[i], sizeof(buff[i]));

    //ret = GetIniKeyString(id, "communication_port", filename, &buff[++i]);
    //printf("%d %s\n", ret, buff[i]);

    ret = GetIniKeyString(id, "control_ip", filename, &buff[++i]);
    printf("%d %s\n", ret, buff[i]);
    memcpy(&info.ip, &buff[i], sizeof(buff[i]));

    ret = GetIniKeyString(id, "control_port", filename, &buff[++i]);
    printf("%d %s\n", ret, buff[i]);

    ret = GetIniKeyString(id, "display_ip", filename, &buff[++i]);
    printf("%d %s\n", ret, buff[i]);
    memcpy(&info.ip, &buff[i], sizeof(buff[i]));

    ret = GetIniKeyString(id, "display_port", filename, &buff[++i]);
    printf("%d %s\n", ret, buff[i]);

}
