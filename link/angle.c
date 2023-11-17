#include <math.h>
#include <stdbool.h>
#include "angle.h"
#include "comcal_dll.h"

typedef double real_T;
typedef float  real32_T;

real_T rtNaN = (real_T)NAN;
real_T rtInf = (real_T)INFINITY;
real_T rtMinusInf = -(real_T)INFINITY;
real32_T rtNaNF = (real32_T)NAN;
real32_T rtInfF = (real32_T)INFINITY;
real32_T rtMinusInfF = -(real32_T)INFINITY;
#define RT_PI                          3.14159265358979323846
#define RT_PIF                         3.1415927F
#define RT_LN_10                       2.30258509299404568402
#define RT_LN_10F                      2.3025851F
#define RT_LOG10E                      0.43429448190325182765
#define RT_LOG10EF                     0.43429449F
#define RT_E                           2.7182818284590452354
#define RT_EF                          2.7182817F


bool rtIsInf(real_T value)
{
    return (isinf(value) != 0U);
}

/* Function: rtIsInfF =================================================
 * Abstract:
 * Test if single-precision value is infinite
 */
bool rtIsInfF(real32_T value)
{
    return (isinf((real_T)value) != 0U);
}

/* Function: rtIsNaN ==================================================
 * Abstract:
 * Test if value is not a number
 */
bool rtIsNaN(real_T value)
{
    return (isnan(value) != 0U);
}

/* Function: rtIsNaNF =================================================
 * Abstract:
 * Test if single-precision value is not a number
 */
bool rtIsNaNF(real32_T value)
{
    return (isnan((real_T)value) != 0U);
}


static double rt_atan2d_snf(double u0, double u1)
{
    double y;
    int b_u0;
    int b_u1;
    if (rtIsNaN(u0) || rtIsNaN(u1)) {
        y = rtNaN;
    }
    else if (rtIsInf(u0) && rtIsInf(u1)) {
        if (u0 > 0.0) {
            b_u0 = 1;
        }
        else {
            b_u0 = -1;
        }

        if (u1 > 0.0) {
            b_u1 = 1;
        }
        else {
            b_u1 = -1;
        }

        y = atan2(b_u0, b_u1);
    }
    else if (u1 == 0.0) {
        if (u0 > 0.0) {
            y = RT_PI / 2.0;
        }
        else if (u0 < 0.0) {
            y = -(RT_PI / 2.0);
        }
        else {
            y = 0.0;
        }
    }
    else {
        y = atan2(u0, u1);
    }

    return y;
}


void calculateAngles(const Point3D* point, double* alpha, double* beta)
{
    double beta_tmp;

    /*  将弧度转换为角度 */
    *alpha = 57.295779513082323 * rt_atan2d_snf(point->y, point->x) + 180;

    beta_tmp = point->x * point->x + point->y * point->y;

    *beta = 57.295779513082323 * asin(sqrt(beta_tmp) / sqrt(beta_tmp + point->z * point->z));
}


/**********************************************************************************************/
/**********************************************************************************************/
/**********************************************************************************************/
//距离计算
double caculate_distance(Point3D a, Point3D b)
{
    double distance = sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2));
    return distance;
}

//时延计算
double caculate_time_delay(double distance)
{
    return distance / C;
}

//路径损耗
double caculate_path_loss(double distance, double frequency)
{
    return pow((4 * PI * distance * frequency / C), 2);
}

//径向速度 b：发射方 a：接收方
double caculate_radial_velocity(Point3D a, Point3D av, Point3D b, Point3D bv)
{
    double Dx = a.x - b.x;
    double Dy = a.y - b.y;
    double Dz = a.z - b.z;
    double Dvx = av.x - bv.x;
    double Dvy = av.y - bv.y;
    double Dvz = av.z - bv.z;
    double v = (Dx * Dvx + Dy * Dvy + Dz * Dvz) / sqrt(pow(Dx, 2) + pow(Dy, 2) + pow(Dz, 2));
    return v;
}

//多普勒频移
double caculate_doppler_shift(double frequency, double v)
{
    return frequency * v / C;
}


//发射功率 
double caculate_transmit_power(double Pti_dBm, double Nt, double Lc)
{
    double Pt_dBm = Pti_dBm + 10 * log10(Nt) - Lc;
    return Pt_dBm;
}

//等效全向辐射功率
double caculate_eirp(double Pt_dBm, double Gt_dBi, double Lt_dB)
{
    double EIRP_dBm = Pt_dBm + Gt_dBi - Lt_dB;
    return EIRP_dBm;
}

//天线增益
double caculate_antenna_gain(double Nt, double Dd, double Ef, double lambda, double Ls)
{
    double Gt_dBi = 10 * log10(4 * PI * Nt * pow(Dd, 2) * Ef / pow(lambda, 2)) - Ls;
    return Gt_dBi;
}


//噪声电平
double caculate_noise_level(double Fn_dB, double Lc_dB, double B_dBHz)
{
    double N_dBm = -174 + Fn_dB + Lc_dB + B_dBHz;
    return N_dBm;
}


//等效扩频倍数
double caculate_SS(double Sos, double Sds)
{
    double equivalentSS = Sos / ((log2(Sos) + 1) * Sds);
    return equivalentSS;
}

//扩频增益
double caculate_spreading_gain(double SS)
{
    double spreadingGain_dB = 10 * log10(SS);
    return spreadingGain_dB;
}

//接收功率
double caculate_received_power(double EIRP_dBm, double Li_dB, double AR_dB, double Gr_dBi, double Lr_dB)
{
    double Pr_dBm = EIRP_dBm - Li_dB - AR_dB + Gr_dBi - Lr_dB;
    return Pr_dBm;
}


//信噪比
double caculate_snr(double Pr_dBm, double C_dB, double N_dBm)
{
    double SNR_dB = Pr_dBm + C_dB - N_dBm;
    return SNR_dB;
}

//误码率
double caculate_ber(double SNR_dB)
{
    double SNR_linear = pow(10, SNR_dB / 10);
    double ber = 0.5 * erfc(sqrt(SNR_linear));
    return ber;
}


//C/N0
double C_NO(double eirp,double gt)
{
    return eirp + gt - 0.5 - 0.5 - 0.5 + 228.6;
}

double caculate_snr_(double c_n0,double B)
{
    return c_n0 / B;
}





//选择天线
int select_antenna(Point3D target_pos)
{
    int antenna_id;
    float azimuth;
    float elevation;

    if (MY_INDEX == 0)
    {
        calculate_ante_angle_coord_m(
            overall_fddi_info[MY_INDEX].pos.x,
            overall_fddi_info[MY_INDEX].pos.y,
            overall_fddi_info[MY_INDEX].pos.z,
            overall_fddi_info[MY_INDEX].q.q0,
            overall_fddi_info[MY_INDEX].q.q1,
            overall_fddi_info[MY_INDEX].q.q2,
            overall_fddi_info[MY_INDEX].q.q3,
            MY_INDEX,
            target_pos.x,
            target_pos.y,
            target_pos.z,
            &antenna_id,
            &azimuth,
            &elevation
        );
    }
    else
    {
        calculate_ante_angle_coord_z(
            overall_fddi_info[MY_INDEX].pos.x,
            overall_fddi_info[MY_INDEX].pos.y,
            overall_fddi_info[MY_INDEX].pos.z,
            overall_fddi_info[MY_INDEX].q.q0,
            overall_fddi_info[MY_INDEX].q.q1,
            overall_fddi_info[MY_INDEX].q.q2,
            overall_fddi_info[MY_INDEX].q.q3,
            MY_INDEX,
            target_pos.x,
            target_pos.y,
            target_pos.z,
            &antenna_id,
            &azimuth,
            &elevation
        );
    }
    return antenna_id;
}

int select_antennaA(int index,Point3D target_pos)
{
    int antenna_id;
    float azimuth;
    float elevation;

    if (index == 0)
    {
        calculate_ante_angle_coord_m(
            overall_fddi_info[index].pos.x,
            overall_fddi_info[index].pos.y,
            overall_fddi_info[index].pos.z,
            overall_fddi_info[index].q.q0,
            overall_fddi_info[index].q.q1,
            overall_fddi_info[index].q.q2,
            overall_fddi_info[index].q.q3,
            index,
            target_pos.x,
            target_pos.y,
            target_pos.z,
            &antenna_id,
            &azimuth,
            &elevation
        );
    }
    else
    {
        calculate_ante_angle_coord_z(
            overall_fddi_info[index].pos.x,
            overall_fddi_info[index].pos.y,
            overall_fddi_info[index].pos.z,
            overall_fddi_info[index].q.q0,
            overall_fddi_info[index].q.q1,
            overall_fddi_info[index].q.q2,
            overall_fddi_info[index].q.q3,
            index,
            target_pos.x,
            target_pos.y,
            target_pos.z,
            &antenna_id,
            &azimuth,
            &elevation
        );
    }
    return antenna_id;
}


//接收天线是否为可收到信息的天线
bool antenna_check(Point3D target_pos)
{
    int antenna_id;
    antenna_id = select_antennaA(MY_INDEX, target_pos);
    if (antenna_id < 0) return false;
    if (display_data.antenna_params[antenna_id].tx_rx_status != 2) return false;
    return true;
}





