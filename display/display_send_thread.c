#include "display_send_thread.h"
#include "file_manage.h"
#include "angle.h"
#include "physical_simulation.h"
#include "display_thread.h"
#include "math.h"
#include "stdio.h"

display_state_t display_state;
show_t show_msg;
static int display_fd;
static int lfd;

typedef void (*display_proc_fun)(show_t*);

int flag_ = 0;

static display_proc_fun fun[] = {
    sim_beg_proc,
    sim_end_proc,
    rep_sel_proc,
    rep_beg_proc,
    rep_rep_proc,
    rep_suspend_proc,
    rep_recover_proc,
};


void* display_send_thread(void* arg)
{
    pthread_detach(pthread_self());

    int ret = 0;
    char data[MAX_DATA_LEN];
    show_t msg;

    init();
    /*以太网连接*/
    display_send_thread_init();


    while (1)
    {
        ret=recv(display_fd, data, MAX_DATA_LEN, 0);
        if (ret > 0)
        {
            show_t* p = (show_t*)data;
            fun[p->type - 101](p);
        }
        else
        {
            //printf("lost display connect\n");
            //display_fd = accept(lfd, (struct sockaddr*)&(info.display_system.addr), &(info.display_system.addr_len));
            //printf("get display connect\n");
            exit(0);
        }
    }
}


static void init()
{
    display_state.interval = 5;
    display_state.mode = 0;
    display_state.seq = 0;
    display_state.find_seq = 0;
    display_state.tx_flag = 0;
    pthread_mutex_init(&display_state.mutex, NULL);
}

void display_send_thread_init()
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
    inet_pton(AF_INET, info.display_ip, (void*)&addr.sin_addr); //**ip**
    addr.sin_port = htons(info.display_port); //**port**
    ret = bind(lfd, (struct sockaddr*)&addr, sizeof addr);

    //开始监听
    listen(lfd, SOMAXCONN);

    info.display_system.addr_len = sizeof(info.display_system.addr); //**
    display_fd = accept(lfd, (struct sockaddr*)&(info.display_system.addr), &(info.display_system.addr_len)); //**
    printf("display_system connect success %d\n", display_fd);
    Sleep(100);
    file_num(display_fd);
    Sleep(100);
    role_id_config();
    Sleep(100);
    generate_key_event(KEY_POWER_ON, 0, 0);
    Sleep(100);
    generate_key_event(KEY_CONFIG_LOAD, 1, 1);


}



void sim_beg_proc(show_t* msg)
{
    display_state.mode = SIM_MODE;
}

void sim_end_proc(show_t* msg)
{

}

//仿真选择
void rep_sel_proc(show_t* msg)
{
    int num=get_file_num(DATA_FOLDER);
    flag_ = 0;
    //msg->file_seq = 0;
    printf("file num=%d select seq=%d\n", num, msg->file_seq);
    //选择文件
    select_file(msg);
    //display_state.mode = REPLAY_MODE;
}

void rep_beg_proc(show_t* msg)
{
    display_state.mode = REPLAY_MODE;
}

void rep_rep_proc(show_t* msg)
{
    int ret;
    ret = fseek(display_state.file, MAX_SEND_LEN * msg->data_seq, SEEK_SET);
    flag_ = 0;
    //display_state.mode = REPLAY_MODE;
    printf("to seq:%d ret=%d\n", msg->data_seq, ret);
}

void rep_suspend_proc(show_t* msg)
{
    display_state.mode = NO_MODE;
}

void rep_recover_proc(show_t* msg)
{
    display_state.mode = REPLAY_MODE;
}


void sim_ready()
{
    show_t msg;
    msg.type = SIM_READY;
    msg.len = 4;
    pthread_mutex_lock(&display_state.mutex);
    send(display_fd, &msg, msg.len, 0);
    pthread_mutex_unlock(&display_state.mutex);
}


void sim_start(uint16_t mode)
{
    show_t msg;
    msg.type = SIM_START;
    msg.len = 4+2;
    msg.mode = mode;
    pthread_mutex_lock(&display_state.mutex);
    send(display_fd, &msg, msg.len, 0);
    pthread_mutex_unlock(&display_state.mutex);
}



void generate_key_event(int type,int id_znum,int role_mnum)
{
    int i;
    show_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.type = IMP_EVENT;
    //msg.len = 4 + sizeof(display_t);

    msg.len = MAX_SEND_LEN;

    //
    msg.key.seq = display_state.seq;
    display_state.seq++;
    msg.key.system_time.tv_sec = my_get_time();
    msg.key.key = type;
    
    msg.key.role = MY_ROLE;
    msg.key.id = MY_INDEX-MY_ROLE;
    msg.key.target_role = role_mnum;
    msg.key.target_id = id_znum - role_mnum;

    if (type == 2)
    {
        msg.key.m_num = role_mnum;
        msg.key.z_num = id_znum;
    }

    if (type == 5)
    {
        online_state[id_znum] = 1;
    }
    else if (type == 6)
    {
        online_state[id_znum] = 0;
    }
    else if (type == 7)
    {
        online_state[id_znum] = 1;
    }

    printf("key event:%d len=%d\n", msg.key.key,msg.len);
    send_to_display(&msg, msg.len);
    todata(&msg, msg.len);
}





#define MAX_PATH_LENGTH 256
void select_file(show_t* msg)
{
    char directory[MAX_PATH_LENGTH];
    strcpy(directory, DATA_FOLDER);

    int fileCount = 0;
    int targetFileIndex = msg->file_seq;
    int currentIndex = 0;

    // 构建目录搜索模式
    char searchPattern[MAX_PATH_LENGTH];
    snprintf(searchPattern, sizeof(searchPattern), "%s/*.txt", directory);

#ifdef _WIN32
    // Windows下的代码
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPattern, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        plog("dir can not find!\n");
        return 1;
    }

    int txtCount = 0;  // txt 文件数量
    do {
        if (findData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {  // 仅处理普通文件
            txtCount++;
        }
    } while (FindNextFileA(hFind, &findData));

    if (txtCount < targetFileIndex)
    {
        printf("error file seq\n");
        return;
    }

    hFind = FindFirstFile(searchPattern, &findData);
    do {
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        if (currentIndex == targetFileIndex) {
            // 构建完整的文件路径
            char filePath[MAX_PATH_LENGTH];
            snprintf(filePath, sizeof(filePath), "%s/%s", directory, findData.cFileName);

            // 打开文件
            display_state.file = fopen(filePath, "rb");
            if (display_state.file == NULL) {
                plog("file can not open:%s\n", filePath);
                FindClose(hFind);
                return 1;
            }
            printf("open file:%s\n", filePath);
            break;
        }

        currentIndex++;
    } while (FindNextFile(hFind, &findData));

    FindClose(hFind);

#else
    // Linux下的代码
    FILE* pipe;
    char command[MAX_PATH_LENGTH + 8];  // "ls -1 <dir>"
    snprintf(command, sizeof(command), "ls -1 %s", searchPattern);

    // 执行ls命令获取文件列表
    pipe = popen(command, "r");
    if (pipe == NULL) {
        plog("无法打开目录！\n");
        return 1;
    }

    char fileName[MAX_PATH_LENGTH];
    while (fgets(fileName, sizeof(fileName), pipe) != NULL) {
        // 删除文件名末尾的换行符
        fileName[strcspn(fileName, "\n")] = '\0';

        if (currentIndex == targetFileIndex) {
            // 构建完整的文件路径
            char filePath[MAX_PATH_LENGTH];
            snprintf(filePath, sizeof(filePath), "%s/%s", directory, fileName);

            // 打开文件
            FILE* file = fopen(filePath, "r");
            if (file == NULL) {
                plog("无法打开文件：%s\n", filePath);
                pclose(pipe);
                return 1;
            }

            // 在这里处理或使用所打开的文件
            // ...

            // 关闭文件
            fclose(file);

            // 找到目标文件后退出循环
            break;
        }

        currentIndex++;
    }

    pclose(pipe);

#endif

    return;
}

void set_zero(show_t* msg)
{
    int i, j;
    memset(msg, 0, sizeof(show_t));
}



void find_data()
{
    show_t msg;
    int ret = 0;
    if (flag_ == 1 && msg.len == 0)
    {
        return;
    }
    ret = fread(&msg, 1, MAX_SEND_LEN, display_state.file);

    if (msg.type == 0) msg.type = 6;
    if (msg.type == 5) msg.type = 7;
    printf("flag=%d len=%d\n", flag_, msg.len);
    enqueue(&info.thread_queue[DISPLAY_RECV_THREAD], &msg, ret);
    if (msg.len == 0)
    {
        flag_ = 1;
    }
}



void send_to_display(char* data,int len)
{
    int i;
    pthread_mutex_lock(&display_state.mutex);
    send(display_fd, data, len, 0);
    //for (i = 0; i < len; i++)
    //{
    //    printf("%02x ", (uint8_t)data[i]);
    //}
    //printf("\n");
    pthread_mutex_unlock(&display_state.mutex);

}



void file_num(int fd)
{
    show_t msg;
    memset(&msg, 0, sizeof(show_t));
    int i;
    msg.type = 4;
    msg.file_info.file_num = get_file_num("C:\\Digital prototype\\data");
    msg.len = 6 + 30 * msg.file_info.file_num;
    
    for (i = 0; i < msg.file_info.file_num; i++)
    {
        sprintf(msg.file_info.file_name[i], "%d:", i);
        printf("file name:%s\n", msg.file_info.file_name[i]);
    }

    send(fd, &msg, msg.len, 0);
}

void net_num(int fd, int m, int z)
{
    show_t msg;
    memset(&msg, 0, sizeof(show_t));
    msg.type = 10;
    msg.len = 6;
    msg.num.m_num = m;
    msg.num.z_num = z;
    send(fd, &msg, msg.len, 0);
}

void role_id_config()
{
    show_t msg;
    memset(&msg, 0, sizeof(show_t));
    msg.type = ROLE_CONFIG;
    msg.len = 1024;
    msg.roleid.role = MY_ROLE;
    msg.roleid.id = MY_INDEX - MY_ROLE;
    send(display_fd, &msg, msg.len, 0);
}









