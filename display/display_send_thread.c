#include "display_send_thread.h"
#include "file_manage.h"
#include "angle.h"
#include "physical_simulation.h"
#include "display_thread.h"

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
    //file_num(display_fd);
    net_num(display_fd, 1, 1);


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
            printf("lost display connect\n");
            display_fd = accept(lfd, (struct sockaddr*)&(info.display_system.addr), &(info.display_system.addr_len));
            printf("get display connect\n");
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
    printf("file num=%d select seq=%d\n", num, msg->file_seq);
    //选择文件
    select_file(msg);
}

void rep_beg_proc(show_t* msg)
{
    display_state.mode = REPLAY_MODE;
}

void rep_rep_proc(show_t* msg)
{
    int ret;
    ret=fseek(display_state.file, (4+sizeof(display_t)) * msg->data_seq, SEEK_SET);
    flag_ = 0;
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

static float  posx = 0;
static float  posy = 0;
static float  posz = 0;
static double eirp = 0;
static uint16_t send_= 0;
static uint16_t recv_ = 0;

static double pr = 0.1;
static double er = 0.1;

int x = 0;
int d0 = 1;
int d1d2 = 3;
int d3d4 = 1;
int d5d10 = 22;
int d11d13 = 6;
int d14d16 = 1;
int d17 = 1;
int d18 = 1;
int d19 = 1;
int d2022 = 3;
int d23 = 1;
int d24 = 1;
int d25d31 = 0;
void send_display_msg()
{
    static float p=0, pp=0;
    show_t msg;
    set_zero(&msg);
    int i;
    int j;
    msg.type = DISPLAY_INFO;
    msg.len = 4 + sizeof(display_t);

    pthread_mutex_lock(&display_state.mutex);
    msg.display_info.serial_number = display_state.seq;
    display_state.seq++;
    msg.display_info.system_time.tv_sec = x * 20 * 1000 + 1688610381000000000;// 假设系统时间为 2021-06-21 12:00:00
    msg.display_info.system_time.tv_nsec = 0;

    //位置信息
    msg.display_info.pos_x = overall_fddi_info[0].pos.x/100;
    msg.display_info.pos_y = overall_fddi_info[0].pos.y;
    msg.display_info.pos_z = overall_fddi_info[0].pos.z/100;
    //printf("%f\n", overall_fddi_info[0].pos.y);

    msg.display_info.vel_x = 1.0;
    msg.display_info.vel_y = 2.0;
    msg.display_info.vel_z = 3.0;
    msg.display_info.ang_vel_x = 0.1;
    msg.display_info.ang_vel_y = 0.2;
    msg.display_info.ang_vel_z = 0.3;
    msg.display_info.quat_q0 = 0.707;
    msg.display_info.quat_q1 = 0.0;
    msg.display_info.quat_q2 = 0.0;
    msg.display_info.quat_q3 = 0.707;
    msg.display_info.time_element_number = 0;
    msg.display_info.time_frame_number = 0;
    msg.display_info.micro_time_slot_number = 0;
    msg.display_info.node_role = 0;
    msg.display_info.node_id = 0;
    msg.display_info.link_status = 1;


    //for (j = 0; j < 5; j++)
    //{
    //    double distance = caculate_distance(fddi_info.pos, overall_fddi_info[1].pos);
    //    double alpha, beta;
    //    Point3D pos2;
    //    convertCoordinates(&overall_fddi_info[1].pos, &fddi_info.q, &pos2);
    //    calculateAngles(&pos2, &alpha, &beta);
    //}

    //与其他节点信息
    if (MY_INDEX == 0){
        double distance = caculate_distance(fddi_info.pos, overall_fddi_info[1].pos);
        double alpha, beta;
        Point3D pos2;
        //对方位置转换到本节点载体坐标系
        convertCoordinates(&overall_fddi_info[1].pos, &fddi_info.q, &pos2);
        //计算方位角俯仰角
        calculateAngles(&pos2, &alpha, &beta);

        msg.display_info.z1_m_distance[1] = distance/20;
        /*msg.display_info.z1_m_azimuth[1] = alpha+90;
        msg.display_info.z1_m_elevation[1] = beta;*/
        msg.display_info.z1_m_azimuth[1] = fmin(p++,45);
        msg.display_info.z1_m_elevation[1] = fmin(pp++, 45);

        //msg.display_info.z1_m_distance[4] = distance / 20;
        //msg.display_info.z1_m_azimuth[4] = fmin(p++, 45);
        //msg.display_info.z1_m_elevation[3] = fmin(pp++, 45);

        //printf("%f %f %f\n", distance, alpha, beta);
        //tosche("%f %f %f\n", msg.display_info.pos_x,
        //    msg.display_info.pos_y,
        //    msg.display_info.pos_z
        //);
        //printf("%f %f %f %f %f %f\n", msg.display_info.pos_x,
        //    msg.display_info.pos_y,
        //    msg.display_info.pos_z,
        //    msg.display_info.z1_m_distance[1],
        //    msg.display_info.z1_m_azimuth[1],
        //    msg.display_info.z1_m_elevation[1]
        //);
    }
    else if (MY_INDEX == 1)
    {
        double distance = caculate_distance(fddi_info.pos, overall_fddi_info[0].pos);
        double alpha, beta;
        Point3D pos2;
        //对方位置转换到本节点载体坐标系
        convertCoordinates(&overall_fddi_info[1].pos, &overall_fddi_info[0].q, &pos2);
        //计算方位角俯仰角
        calculateAngles(&pos2, &alpha, &beta);

        msg.display_info.z1_m_distance[1] = distance/20;
        msg.display_info.z1_m_azimuth[1] = alpha;
        msg.display_info.z1_m_elevation[1] = beta;

    }


    msg.display_info.comm_status_mode = 2;
    msg.display_info.z_proc_flight_control_data_rx_tx_count = (send_ << 16) | recv_;
    send_ += 2;
    recv_++;
    msg.display_info.z_proc_flight_control_data_rx_tx_timestamp = 0;
    //msg.display_info.z1_m_air_interface_data_rx_tx_count = 0;
    for (i = 0; i < 4; i++)
    {
        msg.display_info.z_m_send_recv_count[i] = 0;
    }
    //msg.display_info.z1_m_air_interface_data_rx_tx_timestamp = 0;
    msg.display_info.operation_status = 0;
    msg.display_info.channel_coding_decoding_frame_count = 0;
    msg.display_info.modulation_demodulation_frame_count = 0;
    msg.display_info.instruction_parsing_frame_count = 0;
    msg.display_info.m_node_time_freq_sync_status = 0;
    msg.display_info.m_node_downlink_link_status = 0;
    msg.display_info.m_node_beam_azimuth_direction = 0;
    msg.display_info.m_node_beam_elevation_direction = 90;


    msg.display_info.frequency_synthesizer_status = 0;
    msg.display_info.terminal_working_status_representation = 0;
    int temp = 0;
    temp |= d0;
    temp |= (d1d2 << 1);
    temp |= (d3d4 << 3);
    temp |= (d5d10 << 5);
    temp |= (d11d13 << 11);

    temp |= (d14d16 << 14);
    temp |= (d17 << 17);
    temp |= (d18 << 18);
    temp |= (d19 << 19);
    temp |= (d2022 << 20);
    temp |= (d23 << 23);
    temp |= (d24 << 24);

    msg.display_info.terminal_working_status_representation = temp;

    for (j = 0; j < 6; j++)
    {
        msg.display_info.antenna_params[j].tx_rx_status = 1;
        msg.display_info.antenna_params[j].beam_width = 30;
        msg.display_info.antenna_params[j].azimuth = 45.67;
        msg.display_info.antenna_params[j].elevation = 12.34;
        msg.display_info.antenna_params[j].eirp = eirp++;
        msg.display_info.antenna_params[j].gt = eirp++;
    }
    for (j = 0; j < 4; j++)
    {
        msg.display_info.channel_params[j].node = 1;
        msg.display_info.channel_params[j].packet_loss_rate = pr;
        msg.display_info.channel_params[j].error_rate = pr;
        pr += 0.01;
        if (pr < 0.9)
        {
            pr += 0.01;
        }
        else
        {
            pr -= 0.8;
        }
        msg.display_info.channel_params[j].snr = 20.5;
        msg.display_info.channel_params[j].received_signal_power = -70.2;
        msg.display_info.channel_params[j].spreading_gain = 12.3;
        msg.display_info.channel_params[j].equivalent_spreading_factor = 4.5;
        msg.display_info.channel_params[j].noise_level = -90.1;
        msg.display_info.channel_params[j].distance = 500.0;
        msg.display_info.channel_params[j].path_loss = 60.8;
        msg.display_info.channel_params[j].transmission_delay = 0.003;
        msg.display_info.channel_params[j].doppler_shift = 100.5;
        msg.display_info.channel_params[j].radial_velocity = 50.2;
        msg.display_info.channel_params[j].beam_angle = 30.0;
        msg.display_info.channel_params[j].antenna_gain = 18.7;
        msg.display_info.channel_params[j].equivalent_isotropic_radiated_power = 30.5;
        msg.display_info.channel_params[j].transmitter_output_power = 25.8;
        if (j == 0)
            msg.display_info.channel_params[j].state = 1;
        if (j == 1)
            msg.display_info.channel_params[j].state = 0;
        if (j == 2)
            msg.display_info.channel_params[j].state = 1;
        if (j == 3)
            msg.display_info.channel_params[j].state = 0;
    }


    create_table(&msg);
    for (i = 0; i < 5; i++)
    {
        printf("%d ", online_state[i]);
    }
    printf("\n");

    for (i = 0; i < 2; i++)
    {
        for (j = 0; j < 6; j++)
        {
            printf("%d ", msg.display_info.link_target[i][j]);
        }
        printf("\n");
    }
    printf("****\n");


    send(display_fd, &msg, msg.len, 0);
    todata(&msg, msg.len);
    //enqueue(&info.thread_queue[DISPLAY_RECV_THREAD], &msg, msg.len);
    pthread_mutex_unlock(&display_state.mutex);
}


void generate_key_event(int type,int id,int role)
{
    int i;
    show_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.type = IMP_EVENT;
    msg.len = 4 + sizeof(display_t);

    //
    pthread_mutex_lock(&display_state.mutex);
    msg.key.seq = display_state.seq;
    display_state.seq++;
    msg.key.system_time.tv_sec = my_get_time();
    msg.key.key = type;
    
    msg.key.role = info.device_info.node_role;
    msg.key.id = MY_INDEX;
    msg.key.target_role = role;
    msg.key.target_id = id;
    if (type == 5)
    {
        online_state[id] = 1;
    }
    else if (type == 6)
    {
        online_state[id] = 0;
    }
    else if (type == 7)
    {
        online_state[id] = 1;
    }


    memcpy(&msg.key.pos_x, &fddi_info.pos.x, sizeof(float) * 13);
    msg.key.pos_x = overall_fddi_info[0].pos.x / 100;
    msg.key.pos_y = overall_fddi_info[0].pos.y;
    msg.key.pos_z = overall_fddi_info[0].pos.z / 100;
    printf("key event:%d\n", msg.key.key);

    send(display_fd, &msg, msg.len, 0);
    todata(&msg, msg.len);
    //enqueue(&info.thread_queue[DISPLAY_RECV_THREAD], &msg, msg.len);
    pthread_mutex_unlock(&display_state.mutex);
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
    //for (i = 0; i < 5; i++)
    //{
    //    for (j = 0; j < 6; j++)
    //    {
    //        msg->display_info.link_target[i][j] = -1;
    //    }
    //}
}


void find_data()
{
    show_t msg;
    int ret = 0;
    if (flag_ == 1 && msg.len == 0)
    {
        return;
    }
    ret=fread(&msg, 1, 4 + sizeof(display_t), display_state.file);

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
    send(display_fd, data, len, 0);
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
        //printf("file name:%s\n", msg.file_info.file_name[i]);
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









