#include "display_send_thread.h"


display_state_t display_state;
show_t show_msg;

void* display_send_thread(void* arg)
{
    int ret = 0;
    char data[MAX_DATA_LEN];
    show_t msg;
    msg.file_seq = 0;


    find_data();
    //select_file(&msg);

    display_state.interval = 20;
    display_state.seq = 0;

    pthread_detach(pthread_self());

    /*以太网连接*/
    display_send_thread_init();

    while (1)
    {
        ret=recv(DISPLAY_FD, data, MAX_DATA_LEN, 0);
        if (ret > 0)
        {
            show_t* p = (show_t*)data;
            recv_fr_display(p);
        }

        if (display_state.mode == SIM_MODE && display_state.tx_flag == ON)
        {
            send_to_display(&show_msg, DISPLAY_INFO, sizeof(display_t));
            Sleep(display_state.interval);
        }
        else if (display_state.mode == REPLAY_MODE && display_state.tx_flag == ON)
        {
            find_data();
            Sleep(display_state.interval);
        }
    }

}




void display_send_thread_init()
{
    int lfd;
    int ret = 0;

    //创建侦听socket
    lfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //设置端口复用
    int opt = 1;
    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) == -1) printf("setsockopt error");

    //绑定本机ip地址、端口号
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, info.display_ip, (void*)&addr.sin_addr); //**ip**
    addr.sin_port = htons(info.display_port); //**port**
    ret = bind(lfd, (struct sockaddr*)&addr, sizeof addr);

    //开始监听
    listen(lfd, SOMAXCONN);

    info.display_system.addr_len = sizeof(info.display_system.addr); //**
    info.display_system.fd = accept(lfd, (struct sockaddr*)&(info.display_system.addr), &(info.display_system.addr_len)); //**
    printf("display_system connect success %d\n", info.display_system.fd);
    setNonBlocking(info.display_system.fd);
}


void send_to_display(show_t* msg,int type,int len)
{
    msg->type = type;
    msg->len = 4 + len;
    switch (type)
    {
        case DISPLAY_INFO:
            break;
        case SIM_READY:
            break;
        case SIM_START:
            break;
        case SIM_END:
            break;
        case FILE_SEQ:
            break;
        case IMP_EVENT:
            break;
    }
}

void recv_fr_display(show_t* msg)
{
    switch (msg->type)
    {
        case SIM_START_:
            set_mode(SIM_MODE, ON);
            break;
        case SIM_END_:
            set_mode(NO_MODE, OFF);
            break;
        case REPLAY_SELECT_:
            select_file(msg);
            break;
        case REPLAY_START_:
            set_mode(REPLAY_MODE, ON);
            break;
        case REPLAY_REP_:
            display_state.seq = msg->data_seq;
            break;
        case REPLAY_STOP_:
            set_mode(REPLAY_MODE, OFF);
            break;
        case REPLAY_RECOVER_:
            set_mode(REPLAY_MODE, ON);
            break;
        case REPLAY_SPEED:

            break;
    }
}


void set_mode(int mode,int flag)
{
    display_state.mode = mode;
    display_state.tx_flag = flag;
}


#define MAX_PATH_LENGTH 256
void select_file(show_t* msg)
{
    char directory[MAX_PATH_LENGTH] = "C:\\Users\\MRGT\\source\\repos\\for_display\\name";
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
        printf("dir can not find!\n");
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
            display_state.file = fopen(filePath, "r");
            if (display_state.file == NULL) {
                printf("file can not open:%s\n", filePath);
                FindClose(hFind);
                return 1;
            }

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
        printf("无法打开目录！\n");
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
                printf("无法打开文件：%s\n", filePath);
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

    return ;
}


void find_data()
{
    show_t msg;
    fseek(display_state.file, sizeof(show_t) * display_state.seq++, SEEK_SET);
    fread(&msg, sizeof(char), sizeof(show_t), display_state.file);
    //发送
}

//void find_data()
//{
//    char a[1024];
//    FILE* file;
//    file = fopen("C:\\Users\\MRGT\\Desktop\\display.txt", "rwb");  // 以二进制写入模式打开文件
//    if (file == NULL) {
//        printf("open fail\n");
//        return 1;
//    }
//
//    int i;
//    for (i = 0; i < 10; i++)
//    {
//        memset(a, i, 1024);
//        fwrite(a, sizeof(char), sizeof(a), file);  // 将内容写入文件
//        printf("write success\n");
//    }
//
//    fseek(file, 2*1024, SEEK_SET);
//    fread(a, sizeof(char), sizeof(a), file);
//    printf("five \n");
//    for (i = 0; i < sizeof(a); i++) {
//        printf("%x ", a[i]);
//    }
//
//
//    return 0;
//}










