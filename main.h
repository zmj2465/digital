#ifndef _MAIN_H_
#define _MAIN_H_


void load_simulation_config();
void data_store_init();


int GetIniKeyString(char* title, char* key, char* filename, char* buf);
int load_config(char* filename);

#endif