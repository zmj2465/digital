#include <math.h>
#include <stdbool.h>
#include "angle.h"

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
