#ifndef _ANGLE_H_
#define _ANGLE_H_

#include "common.h"
#include "math.h"

#define C  299792458.0
#define PI 3.1415926

void calculateAngles(const Point3D* point, double* alpha, double* beta);

double caculate_distance(Point3D a, Point3D b);

#endif