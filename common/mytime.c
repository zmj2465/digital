#include "mytime.h"
#include "stdio.h"

typedef BOOL(*FUNC)(void);
typedef LONGLONG(*FUNB)(void);
typedef int(*FUND)(void);
typedef int(*FUNE)(HANDLE hEvent);
typedef int(*FUNF)(void);
typedef int(*FUNG)(ULONG time_interval);
typedef ULONG(*FUNH)(void);

FUNB get_time;


int time_init()
{
    int state;
    const char* dllName = "pcieapi.dll";
    const char* funName1 = "?GetDeviceHandle@@YAHXZ";
    const char* funName2 = "?ReadPcieTime@@YA_KXZ";
    HMODULE hDLL = LoadLibrary(dllName);
    if (hDLL != NULL)
    {
        FUNC init_pcie = GetProcAddress(hDLL, funName1);

        if (init_pcie != NULL)
        {
            state = init_pcie();
            if (state == 0)
            {
                printf("fail\n");
                //cout << "init_pcie failed\n";
            }

            if (state != 0)
            {
                get_time = GetProcAddress(hDLL, funName2);
            }
        }
    }
    else
    {
        printf("fail\n");
    }
	return 0;
}

uint64_t my_get_time()
{
    uint64_t ret=0;
    if (get_time != 0)
    {
        ret = get_time();
    }
    else
    {
        printf("get_time error\n");
    }
    return ret;
}


