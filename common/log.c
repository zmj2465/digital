#include "log.h"

#define DATA_LOG 0
#define STRING_LOG 1


char string_buffer[200];

struct log_info_t log_info[3] = {
	{"C:\\Digital LOG","log",20,},
	{},
	{}
};

sem_t log_semaphore;

void* log_thread(void* arg)
{
	pthread_detach(pthread_self());
	sem_init(&log_semaphore, 0, 0);

	while (1)
	{
		sem_wait(&log_semaphore);
		log_store(0);
	}
}




void log_init()
{
	int i;
	//char folder_name[100];
	char file_name[100];
	for (i = 0; i < sizeof(log_info) / sizeof(log_info[0]); i++)
	{
		sprintf(log_info[i].folder_name, "%s\\%s", log_info[i].path, log_info[i].name);
		mkdir(log_info[i].folder_name);
		sprintf(file_name, "%s\\%d", log_info[i].folder_name, log_info[i].file_num);
		log_info[i].file = fopen(file_name, "w");
		
	}
}


void p_log_dataA(int log_index, int len, uint8_t* data, uint8_t type)
{
	int block = log_info[log_index].current_block;
	int index = log_info[log_index].current_index;
	if (log_info[log_index].current_index + 1 > 1024)
	{
		log_info[log_index].current_block = (log_info[log_index].current_block + 1) % 2;
		log_info[log_index].current_index = 0;
		block = log_info[log_index].current_block;
		index = log_info[log_index].current_index;
		sem_post(&log_semaphore);
	}

	log_info[log_index].log_block[block][index].time = time(NULL);
	log_info[log_index].log_block[block][index].len = len;
	log_info[log_index].log_block[block][index].type = type;
	memcpy(log_info[log_index].log_block[block][index].content, data, len);
	log_info[log_index].current_index++;
}

void p_log_data(int log_index, int len, uint8_t* data)
{
	p_log_dataA(log_index, len, data, DATA_LOG);
}

void p_log_string(int log_index, char* string, ...)
{
	va_list args;
	va_start(args, string);
	
	vsprintf(string_buffer, string, args);
	p_log_dataA(log_index, strlen(string_buffer) + 1, string_buffer, STRING_LOG);
}


void log_store(int log_index)
{
	int i,j;
	char file_name[100];
	int block = (log_info[log_index].current_block + 1) % 2;
	for (i = 0; i < 1024; i++)
	{
		char n[100];
		time_t t = log_info[log_index].log_block[block][i].time;
		struct tm* tm = localtime(&t);
		strftime(n, sizeof(n), "%Y%m%d_%H-%M-%S", tm);
		fprintf(log_info[log_index].file, "[%s]->", n);

		if (log_info[log_index].log_block[block][i].type == DATA_LOG)
		{
			
			for (j = 0; j < log_info[log_index].log_block[block][i].len; j++)
			{
				fprintf(log_info[log_index].file, "%02x ", log_info[log_index].log_block[block][i].content[j]);
			}
			fprintf(log_info[log_index].file, "\n");
		}
		else
		{
			fprintf(log_info[log_index].file, "%s", log_info[log_index].log_block[block][i].content);
		}
		log_info[log_index].file_size += log_info[log_index].log_block[block][i].len;
	}
	if (log_info[log_index].file_size > 1024 * 1024 * 1024)
	{
		fclose(log_info[log_index].file);
		log_info[log_index].file_size = 0;
		log_info[log_index].file_num++;
		sprintf(file_name, "%s\\%d", log_info[i].folder_name, log_info[i].file_num);
		log_info[i].file = fopen(file_name, "w");
	}
}





