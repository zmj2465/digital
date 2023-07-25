#ifndef _TIME__H_
#define _TIME__H_

#include <stdint.h>
#include <Windows.h>   
#include <tchar.h> 
#include <process.h>
#include <time.h>

int time_init();
uint64_t my_get_time();

#endif