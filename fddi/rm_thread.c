#include "rm_thread.h"
#include "rfm2g_api.h"
#if (defined(WIN32))
#include "rfm2g_windows.h"
#endif
#pragma comment(lib,"rfm2gdll_stdc_64.lib")
#define  DEVICE_PREFIX   “\\\\.\\rfm2g”
#define  OFFSET		   2000

void* rm_thread(void* arg)
{
	pthread_detach(pthread_self());
    const char* device = "\\\\.\\rfm2g1";
    RFM2G_STATUS   result;
    RFM2GHANDLE    Handle = 0;
    result = RFM2gOpen((char*)device, &Handle);
    if (result != RFM2G_SUCCESS)
    {
        printf("ERROR: RFM2gOpen() 失败。\n");
        printf("Error: %s.\n\n", RFM2gErrorMsg(result));
        return(-1);
    }
    else
    {
        printf("ERROR: RFM2gOpen() 成功。\n");
    }
    double  Value = 3.1415926;

    /* Write outbuffer into Reflective Memory starting at OFFSET */
    result = RFM2gWrite(Handle, OFFSET, (void*)&Value, sizeof(double));
    if (result == RFM2G_SUCCESS)
    {
        printf("3.1415926成功写入到反射内存卡中.\n");
    }
    else
    {
        printf("ERROR: 数据写入失败。\n");
        RFM2gClose(&Handle);
        return(-1);
    }
    double Value_Rcv;

    /* Got the interrupt, now read data from the other board from OFFSET */
    result = RFM2gRead(Handle, OFFSET, (void*)&Value_Rcv, sizeof(double));
    if (result != RFM2G_SUCCESS)
    {
        printf("\nERROR: 数据读取失败。\n");
        RFM2gClose(&Handle);
        return(-1);
    }
    else {
        printf("\n数据读取成功：Value_Rcv=%1.7lf\n", Value_Rcv);
    }

}