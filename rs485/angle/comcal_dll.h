/********************************************************************************
* @File name: stk_connect.h
* @Author: cjg
* @Version: 1.0
* @Date: 2023��11��16��09:55:26
* @Description: This is a file about exporting function declarations

* @ �����ƣ�-5
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
������Ϣ��1. ���Զ�֡����Ԫ����q0,q1,q2,q3q
          ������ϵλ����Ϣ��PosX��PosY��PosZ
		  2. ����ɨ������ɨ����Ӧ��������ɡ�����ά�����Զ˷���ϵλ����Ϣ��targtFgPosX��targtFgPosY��targtFgPosZ
�����Ϣ��������ѡ�������Ŀ����ѡ��������ĸ����ǡ���λ��
int *antenum,float *Azimuth,float *Elevation
********************************************************************************/
#ifdef  __cplusplus
extern "C" {
#endif
	/*-
	Function name : calculate_ante_angle_coord_z
	Description   : ��ȡZ��
	Parameter
	* Return      : 
	-*/
	K7CTRBOARDDRIVER_API void calculate_ante_angle_coord_z(float PosX,float PosY,float PosZ,float SYq0,float SYq1,float SYq2
		,float SYq3,u8 nodeId,float tgtPosX,float tgtPosY,float tgtPosZ,int *antenum,float *Azimuth,float *Elevation);

	/*-
	Function name : calculate_ante_angle_coord_m
	Description   : ��ȡM��
	Parameter
	* Return      : 
	-*/
	K7CTRBOARDDRIVER_API void calculate_ante_angle_coord_m(float PosX,float PosY,float PosZ,float SYq0,float SYq1,float SYq2
		,float SYq3,u8 nodeId,float tgtPosX,float tgtPosY,float tgtPosZ,int *antenum,float *Azimuth,float *Elevation);
#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */