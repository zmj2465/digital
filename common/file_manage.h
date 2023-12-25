#ifndef _FILE_MANAGE_H_
#define _FILE_MANAGE_H_

#include "common.h"
#include "stdarg.h"



typedef struct {
	char name[FOLDER_NAME_LEN];
	FILE* file;
	HANDLE fileHandle;
	HANDLE mappingHandle;
	LPVOID mappedData;
	int ptr;
	int size;
	pthread_mutex_t lock;
}file_info_t;

//extern file_info_t log_file;
//extern file_info_t data_file;
//extern file_info_t sche_file;

typedef struct {
	char directory[100];
	char name[20];
	file_info_t file;
}file_config_t;


void create_folder();
void create_file(file_info_t* file, char* dir, char* name);
void create_map(file_info_t* file, int size);
int get_file_num(char* dir);
void file_init();
void todata(char* data, int len);

void for_test();

#endif