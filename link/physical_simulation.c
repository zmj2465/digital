#include "physical_simulation.h"
#include "angle.h"

int prepare_simulation = 0;
struct antenna_info_t antenna_info[6];

phy_fun funs[] = {
    config_judge,
    distance_judge,
    antenna_match_,
    channel_sim,
};


double calculateAngle(const Point3D* a, const Point3D* b)
{
    double absxk;
    double b_scale;
    double b_y;
    double scale;
    double t;
    double y;

    /*  计算向量A和向量B的模长 */
    /*  计算夹角（以弧度为单位） */
    /*  将弧度转换为角度 */
    scale = 3.3121686421112381E-170;
    b_scale = 3.3121686421112381E-170;
    absxk = fabs(a->x);
    if (absxk > 3.3121686421112381E-170) {
        y = 1.0;
        scale = absxk;
    }
    else {
        t = absxk / 3.3121686421112381E-170;
        y = t * t;
    }

    absxk = fabs(b->x);
    if (absxk > 3.3121686421112381E-170) {
        b_y = 1.0;
        b_scale = absxk;
    }
    else {
        t = absxk / 3.3121686421112381E-170;
        b_y = t * t;
    }

    absxk = fabs(a->y);
    if (absxk > scale) {
        t = scale / absxk;
        y = y * t * t + 1.0;
        scale = absxk;
    }
    else {
        t = absxk / scale;
        y += t * t;
    }

    absxk = fabs(b->y);
    if (absxk > b_scale) {
        t = b_scale / absxk;
        b_y = b_y * t * t + 1.0;
        b_scale = absxk;
    }
    else {
        t = absxk / b_scale;
        b_y += t * t;
    }

    absxk = fabs(a->z);
    if (absxk > scale) {
        t = scale / absxk;
        y = y * t * t + 1.0;
        scale = absxk;
    }
    else {
        t = absxk / scale;
        y += t * t;
    }

    absxk = fabs(b->z);
    if (absxk > b_scale) {
        t = b_scale / absxk;
        b_y = b_y * t * t + 1.0;
        b_scale = absxk;
    }
    else {
        t = absxk / b_scale;
        b_y += t * t;
    }

    y = scale * sqrt(y);
    b_y = b_scale * sqrt(b_y);
    return 57.295779513082323 * acos(((a->x * b->x + a->y * b->y) + a->z * b->z) / (y * b_y));
}

void convertCoordinates(const Point3D* p1, const Quaternion* quaternion,
    Point3D* p2)
{
    double b_p2_tmp;
    double c_p2_tmp;
    double d_p2_tmp;
    double e_p2_tmp;
    double f_p2_tmp;
    double g_p2_tmp;
    double h_p2_tmp;
    double i_p2_tmp;
    double j_p2_tmp;
    double p2_tmp;

    /*  计算转换矩阵R */
    /*  进行坐标转换 */
    p2_tmp = quaternion->q1 * quaternion->q2;
    b_p2_tmp = quaternion->q0 * quaternion->q3;
    c_p2_tmp = quaternion->q0 * quaternion->q0;
    d_p2_tmp = quaternion->q1 * quaternion->q1;
    e_p2_tmp = quaternion->q2 * quaternion->q2;
    f_p2_tmp = quaternion->q3 * quaternion->q3;
    g_p2_tmp = quaternion->q1 * quaternion->q3;
    h_p2_tmp = quaternion->q0 * quaternion->q2;
    p2->x = ((((c_p2_tmp + d_p2_tmp) - e_p2_tmp) - f_p2_tmp) * p1->x + 2.0 *
        (p2_tmp - b_p2_tmp) * p1->y) + 2.0 * (h_p2_tmp + g_p2_tmp) * p1->z;
    i_p2_tmp = quaternion->q0 * quaternion->q1;
    j_p2_tmp = quaternion->q2 * quaternion->q3;
    c_p2_tmp -= d_p2_tmp;
    p2->y = (2.0 * (p2_tmp + b_p2_tmp) * p1->x + ((c_p2_tmp + e_p2_tmp) - f_p2_tmp)
        * p1->y) + 2.0 * (j_p2_tmp - i_p2_tmp) * p1->z;
    p2->z = (2.0 * (g_p2_tmp - h_p2_tmp) * p1->x + 2.0 * (i_p2_tmp + j_p2_tmp) *
        p1->y) + ((c_p2_tmp - e_p2_tmp) + f_p2_tmp) * p1->z;
}



void convertCoordinates2(const Point3D* p1, const AntennaTransform* transform, Point3D* p2) {
    double x1 = p1->x;
    double y1 = p1->y;
    double z1 = p1->z;
    double dx = transform->dx;
    double dy = transform->dy;
    double dz = transform->dz;
    double h[3][3] = {
        {transform->h[0][0], transform->h[0][1], transform->h[0][2]},
        {transform->h[1][0], transform->h[1][1], transform->h[1][2]},
        {transform->h[2][0], transform->h[2][1], transform->h[2][2]}
    };

    // 进行坐标转换
    p2->x = dx + x1 * h[0][0] + y1 * h[0][1] + z1 * h[0][2];
    p2->y = dy + x1 * h[1][0] + y1 * h[1][1] + z1 * h[1][2];
    p2->z = dz + x1 * h[2][0] + y1 * h[2][1] + z1 * h[2][2];
}


#define ALPHA_MAX   360
#define ALPHA_MIN   0

#define BETA_MAX    60
#define BETA_MIN    0

/*
* 选择天线:
* my_role:自己节点角色
* my_quaternion:自己四元数
* pos:对方位置
* 返回
* index:选择的天线号
* 天线指向存在数组中
*/


/*
* p11:发射方所在位置
* quaternion1:发射方四元数
* p1_to:发射方天线指向
* index1:发射方所用天线
* role1:发射方角色
* 
* p21:接收方所在位置
* p2_to:接收方天线指向
* quaternion2:接收方四元数
* index2:接收方所用天线
* role2:接收方角色
*/
bool checkAngles(Point3D p11, Point3D p1_to, Quaternion quaternion1, int index1, int role1, Point3D p21, Point3D p2_to, Quaternion quaternion2, int index2, int role2)
{
    int i;
    Point3D p12, p13, p22, p23;
    double distance = 0;
    double anglem;
    double anglez;
    double alpha1[6];
    double beta1[6];
    double alpha2[6];
    double beta2[6];

    double distancex = p11.x - p21.x;
    double distancey = p11.y - p21.y;
    double distancez = p11.z - p21.z;
    distance = sqrt(distancex * distancex + distancey * distancey + distancez * distancez);

    bool ret = false;
    bool ret_send = false;
    bool ret_recv = false;

    if (distance < 500)
    {
        anglem = 60;
        anglez = 60;
    }
    else if (distance < 5000)
    {
        anglem = 25;
        anglez = 60;
    }
    else if (distance < 15000)
    {
        anglem = 25;
        anglez = 50;
    }
    else if (distance < 30000)
    {
        anglem = 25;
        anglez = 25;
    }
    else if (distance < 500000)
    {
        anglem = 8.5;
        anglez = 19;
    }

    convertCoordinates(&p21, &quaternion1, &p22);
    convertCoordinates(&p11, &quaternion2, &p12);

    //发射方判断 p1_to天线指向，p23所在位置夹角 小于波束宽度
    int add = role1 == 0 ? 0 : 6;
    double cmp = role1 == 0 ? anglem : anglez;
    convertCoordinates2(&p22, &transform[index1 + add], &p23);
    double angle = calculateAngle(&p23, &p1_to);
    if (angle < cmp) ret_send = true;

    //接收方判断 p2_to天线指向，p13所在位置夹角 小于波束宽度
    add = role2 == 0 ? 0 : 6;
    cmp = role2 == 0 ? anglem : anglez;
    convertCoordinates2(&p12, &transform[index2 + add], &p13);
    angle = calculateAngle(&p13, &p2_to);
    if (angle < cmp) ret_recv = true;

    return ret_send & ret_recv;
}



bool psy_recv_(psy_msg_t* data, msg_t* msg)
{
    bool ret = true;
    int i = 0;
    for (i = 0; i < sizeof(funs) / sizeof(phy_fun); i++)
    {
        ret |= funs[i](data);
        if (ret == false) return ret;
    }

    return ret;
}



int psy_send_(psy_msg_t* data, msg_t* msg)
{
    data->psy_head.len = sizeof(psy_head_t) + sizeof(msg_t);
    data->psy_head.role = info.device_info.node_role;
    data->psy_head.antenna_id = info.current_antenna;
    data->psy_head.index = MY_INDEX;
    data->psy_head.flag = prepare_simulation;
    //data->psy_head.pos = fddi_info.pos;
    data->psy_head.v = fddi_info.v;
    data->psy_head.rv = fddi_info.rv;
    data->psy_head.q = fddi_info.q;
    memcpy(&data->psy_head.pos, &fddi_info.pos, sizeof(Point3D));
    data->psy_head.p_to = antenna_info[info.current_antenna].point_to;
    data->msg = *msg;
}
//

//信息加载判断 有一方未加载数据则不接收数据
bool config_judge(psy_msg_t* p)
{
    bool ret = true;
    if (p->psy_head.flag == 0 || prepare_simulation == 0) return false;
    return ret;
}

//距离判断 大于最大距离不接收数据
bool distance_judge(psy_msg_t* p)
{
    bool ret = true;
    double distance = caculate_distance(p->psy_head.pos, fddi_info.pos);
    if (distance > MAX_DISTANCE) return false;
    return ret;
}


//天线匹配 匹配失败不接收数据
bool antenna_match_(psy_msg_t* p)
{
    bool ret = true;
    ret = antenna_check(p->psy_head.index, p->psy_head.antenna_id, p->psy_head.pos);
    return ret;
}


//信道仿真 丢包则不接收数据
bool channel_sim(psy_msg_t* data)
{
    bool ret;

    double c_n0 = C_NO(24, -36);
    double snr;
    //信噪比
    if (MY_INDEX == 0)
    {
        snr = caculate_snr_(c_n0, 10 * 1000000);
    }
    else
    {
        snr = caculate_snr_(c_n0, 20 * 1000000);
    }
    //误码率
    double ber = caculate_ber(snr);
    //误帧率
    double fer = 1 - pow(1 - ber, data->msg.len);

    srand(time(NULL));
    int temp = rand()%100;
    int cmp = (1 - fer) * 100;
    if (temp < cmp)
    {
        ret = true;
    }
    else
    {
        ret = false;
    }
    return ret;
}


bool new_angle_check(Point3D send_p, Point3D recv_p, Quaternion send_q, Quaternion recv_q, int send_r, int recv_r, int send_i, int recv_i)
{
    bool ret_send = false;
    bool ret_recv = false;
    Point3D null = { 0,0,1 };
    double angle, anglem, anglez;
    double distancex = send_p.x - recv_p.x;
    double distancey = send_p.y - recv_p.y;
    double distancez = send_p.z - recv_p.z;
    double distance = sqrt(distancex * distancex + distancey * distancey + distancez * distancez);

    anglem = (distance < 500) ? 55 : (distance < 5000) ? 25 : (distance < 15000) ? 25 : (distance < 30000) ? 25 : 8.5;
    anglez = (distance < 500) ? 55 : (distance < 5000) ? 55 : (distance < 15000) ? 50 : (distance < 30000) ? 25 : 19;

    Point3D recv_p2, send_p2, recv_p3, send_p3;
    //发射方
    convertCoordinates(&recv_p, &send_q, &recv_p2);
    convertCoordinates2(&recv_p2, &transform[send_i + (send_r == 0 ? 0 : 6)], &recv_p3);
    angle = calculateAngle(&recv_p3, &null);
    if (angle < (send_r == 0 ? anglem : anglez))
    {
        ret_send = true;
    }

    //接收方
    convertCoordinates(&send_p, &recv_q, &send_p2);
    convertCoordinates2(&send_p2, &transform[recv_i + (recv_r == 0 ? 0 : 6)], &send_p3);
    angle = calculateAngle(&send_p3, &null);
    if (angle < (recv_r == 0 ? anglem : anglez))
    {
        ret_recv = true;
    }

    return ret_send & ret_recv;
}



void fddi_load(fddi_info_t* fddi, psy_msg_t* msg)
{
    memcpy(&fddi->pos, &msg->psy_head.pos, sizeof(Point3D));
    memcpy(&fddi->q, &msg->psy_head.q, sizeof(Quaternion));
    //printf("%f %f %f\n", fddi->pos.x, fddi->pos.y, fddi->pos.z);
}



// 函数定义：计算两个向量的差
Point3D subtractVectors(Point3D v2, Point3D v1) {
    Point3D result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}

// 函数定义：将点绕飞行器的旋转轴进行旋转
Point3D rotatePoint(Point3D point, Quaternion quaternion) {
    Point3D result;

    result.x = (quaternion.q0 * quaternion.q0 + quaternion.q1 * quaternion.q1 - quaternion.q2 * quaternion.q2 - quaternion.q3 * quaternion.q3) * point.x
        + 2 * (quaternion.q1 * quaternion.q2 + quaternion.q0 * quaternion.q3) * point.y
        + 2 * (quaternion.q1 * quaternion.q3 - quaternion.q0 * quaternion.q2) * point.z;

    result.y = 2 * (quaternion.q1 * quaternion.q2 - quaternion.q0 * quaternion.q3) * point.x
        + (quaternion.q0 * quaternion.q0 - quaternion.q1 * quaternion.q1 + quaternion.q2 * quaternion.q2 - quaternion.q3 * quaternion.q3) * point.y
        + 2 * (quaternion.q2 * quaternion.q3 + quaternion.q0 * quaternion.q1) * point.z;

    result.z = 2 * (quaternion.q1 * quaternion.q3 + quaternion.q0 * quaternion.q2) * point.x
        + 2 * (quaternion.q2 * quaternion.q3 - quaternion.q0 * quaternion.q1) * point.y
        + (quaternion.q0 * quaternion.q0 - quaternion.q1 * quaternion.q1 - quaternion.q2 * quaternion.q2 + quaternion.q3 * quaternion.q3) * point.z;

    return result;
}

//计算方位角俯仰角
void calculateYawAndPitch(Point3D my_pos, Quaternion my_q, Point3D target_pos, float* yaw, float* pitch)
{
    Point3D switch_point = subtractVectors(my_pos, target_pos);
    switch_point = rotatePoint(switch_point, my_q);
    //printf("x=%f,y=%f,z=%f\n", my_pos.x, my_pos.y, my_pos.z);
    //printf("x=%f,y=%f,z=%f\n", target_pos.x, target_pos.y, target_pos.z);
    //printf("x=%f,y=%f,z=%f\n", switch_point.x, switch_point.y, switch_point.z);
    *yaw = atan2(switch_point.y, switch_point.x) * (180.0 / 3.1415926);
    *pitch = atan2(-switch_point.z, sqrt(switch_point.x * switch_point.x + switch_point.y * switch_point.y)) * (180.0 / 3.1415926);
    return;
}











