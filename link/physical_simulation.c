#include "physical_simulation.h"

int prepare_simulation = 0;

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


//void convertCoordinates(const Point3D* p1, const Quaternion* quaternion, Point3D* p2) {
//    double x1 = p1->x;
//    double y1 = p1->y;
//    double z1 = p1->z;
//    double q0 = quaternion->q0;
//    double q1 = quaternion->q1;
//    double q2 = quaternion->q2;
//    double q3 = quaternion->q3;
//    // 计算转换矩阵R
//    double R[3][3] = {
//        {q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3, 2 * (q1 * q2 - q0 * q3), 2 * (q0 * q2 + q1 * q3)},
//        {2 * (q1 * q2 + q0 * q3), q0 * q0 - q1 * q1 + q2 * q2 - q3 * q3, 2 * (q2 * q3 - q0 * q1)},
//        {2 * (q1 * q3 - q0 * q2), 2 * (q0 * q1 + q2 * q3), q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3}
//    };
//    // 进行坐标转换
//    p2->x = R[0][0] * x1 + R[0][1] * y1 + R[0][2] * z1;
//    p2->y = R[1][0] * x1 + R[1][1] * y1 + R[1][2] * z1;
//    p2->z = R[2][0] * x1 + R[2][1] * y1 + R[2][2] * z1;
//}

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

//#define PI 3.1415926
//
//double rad2deg(double radian) {
//    return radian * (180.0 / PI);
//}
//
//double calculateAlpha(Point3D point) {
//    double alpha = atan2(point.y, point.x);
//    //double angleInDegrees = rad2deg(alpha);
//
//    //// 将角度限制在 0 到 360 度之间
//    //while (angleInDegrees < 0) {
//    //    angleInDegrees += 360.0;
//    //}
//    //while (angleInDegrees >= 360.0) {
//    //    angleInDegrees -= 360.0;
//    //}
//
//    return alpha;
//}
//
//double calculateBeta(Point3D point) {
//    printf("bb: x=%g y=%g z=%g\n", point.x, point.y, point.z);
//    double beta;
//
//    beta = asin(sqrt(powf(point.x, 2) + powf(point.y, 2)) / sqrt(powf(point.x, 2) + powf(point.y, 2) + powf(point.z, 2)));
//
//    printf("bbb: %g\n", beta);
//    return beta;
//
//}




//bool checkAngles(Point3D p11, Quaternion quaternion1, int index1, int role1, Point3D p21, Quaternion quaternion2, int index2, int role2)
//{
//    Point3D p12, p13, p22, p23;
//    double distance = sqrt(pow(p11.x - p21.x, 2) + pow(p11.y - p21.y, 2) + pow(p11.z - p21.z, 2));
//    double anglem;
//    double anglez;
//    double alpha1;
//    double beta1;
//    double alpha2;
//    double beta2;
//
//
//    bool ret_send = false;
//    bool ret_recv = false;
//
//    if (distance < 500)
//    {
//        anglem = 60;
//        anglez = 60;
//    }
//    else if (distance < 5000)
//    {
//        anglem = 25;
//        anglez = 60;
//    }
//    else if (distance < 15000)
//    {
//        anglem = 25;
//        anglez = 50;
//    }
//    else if (distance < 30000)
//    {
//        anglem = 25;
//        anglez = 25;
//    }
//    else if (distance < 500000)
//    {
//        anglem = 8.5;
//        anglez = 19;
//    }
//
//    //发射天线为原点
//    convertCoordinates(&p21, &quaternion1, &p22);
//    if (role1 == 0)convertCoordinates2(&p22, &transform[index1], &p23);
//    else convertCoordinates2(&p22, &transform[index1 + 6], &p23);
//    alpha1 = calculateAlpha(p23);
//    beta1 = calculateBeta(p23);
//    if (role1 == 0)
//    {
//        if (beta1 < anglem) ret_send = true;
//    }
//    else
//    {
//        if (beta1 < anglez) ret_send = true;
//    }
//
//    //接收天线为原点
//    convertCoordinates(&p11, &quaternion2, &p12);
//    if (role2 == 0)
//    {
//        for (int i = 0; i < 6; i++)
//        {
//            convertCoordinates2(&p12, &transform[i], &p13);
//            alpha2 = calculateAlpha(p13);
//            beta2 = calculateBeta(p13);
//            if (beta2 < anglem)
//            {
//                ret_recv = true;
//                break;
//            }
//        }
//    }
//    else
//    {
//        convertCoordinates2(&p12, &transform[index2 + 6], &p13);
//        alpha2 = calculateAlpha(p13);
//        beta2 = calculateBeta(p13);
//        if (beta2 < anglez) ret_recv = true;
//    }
//    printf("checkAngles:%g %g %g %g %d\n", alpha1, beta1, alpha2, beta2, ret_send & ret_recv);
//    return ret_send & ret_recv;
//}

bool checkAngles(Point3D p11, Quaternion quaternion1, int index1, int role1, Point3D p21, Quaternion quaternion2, int index2, int role2)
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

    //if (p11.x < 0.1 && p11.x>-0.1) p11.x = 0;
    //if (p11.y < 0.1 && p11.y>-0.1) p11.y = 0;
    //if (p11.z < 0.1 && p11.z>-0.1) p11.z = 0;
    //if (p21.x < 0.1 && p21.x>-0.1) p21.x = 0;
    //if (p21.y < 0.1 && p21.y>-0.1) p21.y = 0;
    //if (p21.z < 0.1 && p21.z>-0.1) p21.z = 0;

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

    //printf("[start] %g\n", distance);
    for (i = 0; i < 6; i++)
    {
        if (role1 == 0)
        {
            convertCoordinates2(&p22, &transform[i], &p23);
            calculateAngles(&p23, &alpha1[i], &beta1[i]);
        }
        else if (role1 == 1)
        {
            convertCoordinates2(&p22, &transform[i + 6], &p23);
            calculateAngles(&p23, &alpha1[i], &beta1[i]);
        }
        if (role2 == 0)
        {
            convertCoordinates2(&p12, &transform[i], &p13);
            calculateAngles(&p13, &alpha2[i], &beta2[i]);
        }
        else if (role2 == 1)
        {
            convertCoordinates2(&p12, &transform[i + 6], &p13);
            calculateAngles(&p13, &alpha2[i], &beta2[i]);
        }
        //printf("x2=%g y2=%g z2=%g x1=%g y1=%g z1=%g\n", p23.x, p23.y, p23.z, p13.x, p13.y, p13.z);
        //printf("a1=%g b1=%g a2=%g b2=%g\n", alpha1[i], beta1[i], alpha2[i], beta2[i]);
    }
    //printf("[end]\n");

    if (role1 == 0 && role2 == 1)  //M发 Z收
    {
        if (beta1[index1] < anglem && beta2[index2] < anglez) //m俯仰角满足且z俯仰角满足
        {
            ret = true;
        }
    }
    else if (role1 == 1 && role2 == 0) //Z发 M收
    {
        if (beta1[index1] < anglez) //z俯仰角满足时 遍历m接收天线
        {
            for (i = 0; i < 6; i++) //m只要有一个天线能收到就满足
            {
                if (beta2[i] < anglem)
                {
                    ret = true;
                }
            }
        }
    }
    return ret;

}





void physical_send()
{
	/*crc校验*/
	crc_check();
	/*数据加扰*/
	data_scrambling();
	/*信道编码*/
	channel_coding();
	/*信道交织*/
	channel_interleaving();
	/*数据分组*/
	data_clustering();
	/*组帧*/
	framing();
	/*MSK调制*/
	msk_modulation();
	/*上变频*/
	up_conversion();
	/*信道仿真准备*/

}

void physical_recv()
{
	/*信道仿真*/
	channel_simulation();
	/*滤波*/
	filtering();
	/*同步*/
	synchronous();
	/*解调*/
	msk_demodulation();
	/*解交织*/
	channel_deinterleaving();
	/*信道译码*/
	channel_decode();
	/*数据解扰*/
	data_descrambling();
	/*crc校验*/
	crc_check();
}

/*crc校验*/
void crc_check() {}

/*数据加扰\解扰*/
void data_scrambling() {}
void data_descrambling() {}


/*信道编码\译码*/
void channel_coding() {}
void channel_decode() {}

/*信道交织\解交织*/
void channel_interleaving() {}
void channel_deinterleaving() {}

/*数据分组*/
void data_clustering() {}

/*组帧*/
void framing() {}

/*MSK调制\解调*/
void msk_modulation() {}
void msk_demodulation() {}

/*上变频\下变频*/
void up_conversion() {}
void down_conversion() {}

/*滤波*/
void filtering() {}

/*同步*/
void synchronous() {}


/* */

/*信道仿真*/
void channel_simulation() {}


void transmission_delay() {}
void doppler_shift() {}
void path_loss() {}



int psy_recv(int len,char* data, char* msg, int index, int role)
{
    bool ret;
    psy_msg_t* p = (psy_msg_t*)data;
    //printf("psy_recv:x=%f y=%f z=%f q0=%f q1=%f q2=%f q3=%f\n", p->pos.x, p->pos.y, p->pos.z, p->q.q0, p->q.q1, p->q.q2, p->q.q3);
    if (p->flag == 0 || prepare_simulation == 0) return 1;
    ret=checkAngles(p->pos,p->q,p->index,p->role,fddi_info.pos,fddi_info.q,index,role);
    if (ret == false) return 1;
    memcpy(msg, (char*)(&p->msg), sizeof(msg_t));
    return 0;
}


void psy_send(int len, char* data, char* msg, int index, int role)
{
    psy_msg_t* p= (psy_msg_t*)data;
    memcpy((char*)(&p->msg), msg, sizeof(msg_t));
    p->pos.x = fddi_info.pos.x;
    p->pos.y = fddi_info.pos.y;
    p->pos.z = fddi_info.pos.z;
    p->q.q0 = fddi_info.q.q0;
    p->q.q1 = fddi_info.q.q1;
    p->q.q2 = fddi_info.q.q2;
    p->q.q3 = fddi_info.q.q3;
    p->index = index;
    p->role = role;
    p->flag = prepare_simulation;
}

