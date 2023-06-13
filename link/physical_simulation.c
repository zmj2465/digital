#include "physical_simulation.h"


void convertCoordinates(const Point3D* p1, const Quaternion* quaternion, Point3D* p2) {
    double x1 = p1->x;
    double y1 = p1->y;
    double z1 = p1->z;
    double q0 = quaternion->q0;
    double q1 = quaternion->q1;
    double q2 = quaternion->q2;
    double q3 = quaternion->q3;
    // ����ת������R
    double R[3][3] = {
        {q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3, 2 * (q1 * q2 - q0 * q3), 2 * (q0 * q2 + q1 * q3)},
        {2 * (q1 * q2 + q0 * q3), q0 * q0 - q1 * q1 + q2 * q2 - q3 * q3, 2 * (q2 * q3 - q0 * q1)},
        {2 * (q1 * q3 - q0 * q2), 2 * (q0 * q1 + q2 * q3), q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3}
    };
    // ��������ת��
    p2->x = R[0][0] * x1 + R[0][1] * y1 + R[0][2] * z1;
    p2->y = R[1][0] * x1 + R[1][1] * y1 + R[1][2] * z1;
    p2->z = R[2][0] * x1 + R[2][1] * y1 + R[2][2] * z1;
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

    // ��������ת��
    p2->x = dx + x1 * h[0][0] + y1 * h[0][1] + z1 * h[0][2];
    p2->y = dy + x1 * h[1][0] + y1 * h[1][1] + z1 * h[1][2];
    p2->z = dz + x1 * h[2][0] + y1 * h[2][1] + z1 * h[2][2];
}

#define PI 3.1415926

double rad2deg(double radian) {
    return radian * (180.0 / PI);
}

double calculateAlpha(Point3D point) {
    double alpha = atan2(point.y, point.x);
    return rad2deg(alpha);
}

double calculateBeta(Point3D point) {
    double numerator = sqrt(pow(point.x, 2) + pow(point.y, 2));
    double denominator = sqrt(pow(point.x, 2) + pow(point.y, 2) + pow(point.z, 2));
    double beta = asin(numerator / denominator);
    return rad2deg(beta);
}

//���߶���
//p11���䣬p21����
bool checkAngles(Point3D p11, Quaternion quaternion1, int index1, int role1, Point3D p21, Quaternion quaternion2, int index2, int role2)
{
    Point3D p12, p13, p22, p23;

    //��������Ϊԭ��
    convertCoordinates(&p21, &quaternion2, &p22);
    if (role2 == 0)
    {
        convertCoordinates2(&p22, &transform[index2], &p23);
    }
    else
    {
        convertCoordinates2(&p22, &transform[index2+6], &p23);
    }
    double alpha1 = calculateAlpha(p23);
    double beta1 = calculateBeta(p23);

    //��������Ϊԭ��
    convertCoordinates(&p11, &quaternion1, &p12);
    if (role1 == 0)
    {
        convertCoordinates2(&p12, &transform[index1], &p13);
    }
    else
    {
        convertCoordinates2(&p12, &transform[index1+6], &p13);
    }
    double alpha2 = calculateAlpha(p13);
    double beta2 = calculateBeta(p13);

    printf("checkAngles:%f %f %f %f\n", alpha1, beta1, alpha2, beta2);

    return true;
}




void physical_send()
{
	/*crcУ��*/
	crc_check();
	/*���ݼ���*/
	data_scrambling();
	/*�ŵ�����*/
	channel_coding();
	/*�ŵ���֯*/
	channel_interleaving();
	/*���ݷ���*/
	data_clustering();
	/*��֡*/
	framing();
	/*MSK����*/
	msk_modulation();
	/*�ϱ�Ƶ*/
	up_conversion();
	/*�ŵ�����׼��*/

}

void physical_recv()
{
	/*�ŵ�����*/
	channel_simulation();
	/*�˲�*/
	filtering();
	/*ͬ��*/
	synchronous();
	/*���*/
	msk_demodulation();
	/*�⽻֯*/
	channel_deinterleaving();
	/*�ŵ�����*/
	channel_decode();
	/*���ݽ���*/
	data_descrambling();
	/*crcУ��*/
	crc_check();
}

/*crcУ��*/
void crc_check() {}

/*���ݼ���\����*/
void data_scrambling() {}
void data_descrambling() {}


/*�ŵ�����\����*/
void channel_coding() {}
void channel_decode() {}

/*�ŵ���֯\�⽻֯*/
void channel_interleaving() {}
void channel_deinterleaving() {}

/*���ݷ���*/
void data_clustering() {}

/*��֡*/
void framing() {}

/*MSK����\���*/
void msk_modulation() {}
void msk_demodulation() {}

/*�ϱ�Ƶ\�±�Ƶ*/
void up_conversion() {}
void down_conversion() {}

/*�˲�*/
void filtering() {}

/*ͬ��*/
void synchronous() {}


/* */

/*�ŵ�����*/
void channel_simulation() {}


void transmission_delay() {}
void doppler_shift() {}
void path_loss() {}



int psy_recv(int len,char* data, char* msg, int index, int role)
{
    bool ret;
    psy_msg_t* p = (psy_msg_t*)data;
    printf("psy_recv:x=%f y=%f z=%f q0=%f q1=%f q2=%f q3=%f\n", p->pos.x, p->pos.y, p->pos.z, p->q.q0, p->q.q1, p->q.q2, p->q.q3);
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
}

