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
}file_info_t;

extern file_info_t log_file;
extern file_info_t data_file;



void create_folder();
void create_file(file_info_t* file, char* dir, char* name);
void create_map(file_info_t* file, int size);
int get_file_num(char* dir);
void file_init();
void tolog(char* s, ...);

void for_test();

#endif