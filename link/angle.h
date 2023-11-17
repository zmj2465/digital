#ifndef _ANGLE_H_
#define _ANGLE_H_

#include "common.h"
#include "math.h"
#include <stdbool.h>

#define C  299792458.0
#define PI 3.1415926

void calculateAngles(const Point3D* point, double* alpha, double* beta);

double caculate_distance(Point3D a, Point3D b);


double C_NO(double eirp, double gt);
double caculate_snr_(double c_n0, double B);
double caculate_ber(double SNR_dB);

int select_antenna(Point3D target_pos);
int select_antennaA(int index, Point3D target_pos);
bool antenna_check(int target_index, int target_antenna_id, Point3D target_pos);

#endif