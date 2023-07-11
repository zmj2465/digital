#ifndef _FILE_MANAGE_H_
#define _FILE_MANAGE_H_

#include "common.h"


void create_folder();
void create_file(FILE** file, char* dir, char* name);
int get_file_num(char* dir);
void file_init();

#endif