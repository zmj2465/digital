#include "file_manage.h"

void file_init()
{
    create_folder();
    create_file(&info.data_file, DATA_FOLDER, "data");
    create_file(&info.log_file, LOG_FOLDER, "log");
}


//�ļ��д���
void create_folder()
{
    int i = 0;
    char dirname[][FOLDER_NAME_LEN] = { TOTAL_FOLDER ,LOG_FOLDER,DATA_FOLDER };
    // ʹ��mkdir���������ļ���
    for (i = 0; i < sizeof(dirname) / sizeof(dirname[0]); i++)
    {
        mkdir(dirname[i]);
    }
}

//�����ļ�
void create_file(FILE** file,char* dir,char* name)
{
    char n[100];
    char n1[100];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    strftime(n, sizeof(n), "%Y%m%d_%H-%M-%S", tm);
    sprintf(n1, "%s\\%s_%s.txt", dir, n, name);
    *file = fopen(n1, "w+");
    if (*file == NULL) {
        printf("�޷������ļ�\n");
        return 1;
    }
    return 0;
}


//�ļ������ļ�����
int get_file_num(char* dir)
{
    int txtCount = 0;
    // ����Ŀ¼����ģʽ
    char searchPattern[FOLDER_NAME_LEN];
    snprintf(searchPattern, sizeof(searchPattern), "%s/*.txt", dir);
#ifdef _WIN32
    // Windows�µĴ���
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPattern, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        //printf("dir can not find!\n");
        return 0;
    }
    do {
        if (findData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {  // ��������ͨ�ļ�
            txtCount++;
        }
    } while (FindNextFileA(hFind, &findData));
    FindClose(hFind);
#else
    // Linux�µĴ���
    FILE* pipe;
    char command[MAX_PATH_LENGTH + 8];  // "ls -1 <dir>"
    snprintf(command, sizeof(command), "ls -1 %s", searchPattern);

    // ִ��ls�����ȡ�ļ��б�
    pipe = popen(command, "r");
    if (pipe == NULL) {
        printf("�޷���Ŀ¼��\n");
        return 1;
    }

    char fileName[MAX_PATH_LENGTH];
    while (fgets(fileName, sizeof(fileName), pipe) != NULL) {
        // ɾ���ļ���ĩβ�Ļ��з�
        fileName[strcspn(fileName, "\n")] = '\0';

        if (currentIndex == targetFileIndex) {
            // �����������ļ�·��
            char filePath[MAX_PATH_LENGTH];
            snprintf(filePath, sizeof(filePath), "%s/%s", directory, fileName);

            // ���ļ�
            FILE* file = fopen(filePath, "r");
            if (file == NULL) {
                printf("�޷����ļ���%s\n", filePath);
                pclose(pipe);
                return 1;
            }

            // �����ﴦ���ʹ�����򿪵��ļ�
            // ...

            // �ر��ļ�
            fclose(file);

            // �ҵ�Ŀ���ļ����˳�ѭ��
            break;
        }

        currentIndex++;
    }

    pclose(pipe);

#endif
    return txtCount;
}



