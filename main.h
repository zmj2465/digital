#ifndef _MAIN_H_
#define _MAIN_H_



void thread_init();
void thread_exit_proc();


int GetIniKeyString(char* title, char* key, char* filename, char* buf);
int load_config(char* filename);
void config_init();

#endif