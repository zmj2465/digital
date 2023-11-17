#pragma once

#define K7CTRBOARDDRIVER_EXPORTS

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

	K7CTRBOARDDRIVER_API int my_add(int a, int b);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */