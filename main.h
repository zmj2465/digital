#ifndef _MAIN_H_
#define _MAIN_H_

void load_ip_config();
void load_simulation_config();
void data_store_init();


int GetIniKeyString(char* title, char* key, char* filename, char* buf);

#endif