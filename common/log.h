#ifndef _LOG__H_
#define _LOG__H_

#include <stdint.h>
#include <Windows.h>   
#include <tchar.h> 
#include <process.h>
#include <time.h>
#include "common.h"


struct log_block_t {
	time_t time;
	//uint8_t index;
	uint8_t type;
	int len;
	char content[2000];
};

struct log_info_t {
	char path[30];
	char name[20];
	char folder_name[50];
	struct log_block_t log_block[2][1024];
	int current_block;
	int current_index;
	//int total_index;
	FILE* file;
	int file_num;
	int file_size;
};

void log_init();
void log_store(int log_index);
void* log_thread(void* arg);
void p_log_data(int log_index, int len, uint8_t* data);
void p_log_string(int log_index, char* string, ...);

#endif