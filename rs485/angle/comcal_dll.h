/********************************************************************************
* @File name: stk_connect.h
* @Author: cjg
* @Version: 1.0
* @Date: 2023年11月16日09:55:26
* @Description: This is a file about exporting function declarations

* @ 软件设计：-5
********************************************************************************/

#include <Windows.h>

#ifdef K7CTRBOARDDRIVER_EXPORTS
#define K7CTRBOARDDRIVER_API __declspec(dllexport)
#else
#ifndef K7CTRBOARDDRIVER
#define K7CTRBOARDDRIVER_API __declspec(dllimport)
#else
#define K7CTRBOARDDRIVER_API
#endif
#endif

#define u8 unsigned char
/********************************************************************************
输入信息：1. 来自短帧：四元数：q0,q1,q2,q3q
          自身发惯系位置信息：PosX，PosY，PosZ
		  2. 来自扫描请求、扫描响应、入网许可、网络维护：对端发惯系位置信息：targtFgPosX、targtFgPosY、targtFgPosZ
输出信息：天线面选择情况、目标在选中天线面的俯仰角、方位角
int *antenum,float *Azimuth,float *Elevation
********************************************************************************/
#ifdef  __cplusplus
extern "C" {
#endif
	/*-
	Function name : calculate_ante_angle_coord_z
	Description   : 获取Z的
	Parameter
	* Return      : 
	-*/
	K7CTRBOARDDRIVER_API void calculate_ante_angle_coord_z(float PosX,float PosY,float PosZ,float SYq0,float SYq1,float SYq2
		,float SYq3,u8 nodeId,float tgtPosX,float tgtPosY,float tgtPosZ,int *antenum,float *Azimuth,float *Elevation);

	/*-
	Function name : calculate_ante_angle_coord_m
	Description   : 获取M的
	Parameter
	* Return      : 
	-*/
	K7CTRBOARDDRIVER_API void calculate_ante_angle_coord_m(float PosX,float PosY,float PosZ,float SYq0,float SYq1,float SYq2
		,float SYq3,u8 nodeId,float tgtPosX,float tgtPosY,float tgtPosZ,int *antenum,float *Azimuth,float *Elevation);
#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */