#include "file_manage.h"

void file_init()
{
    create_folder();
    create_file(&info.data_file, DATA_FOLDER, "data");
    create_file(&info.log_file, LOG_FOLDER, "log");
}


//文件夹创建
void create_folder()
{
    int i = 0;
    char dirname[][FOLDER_NAME_LEN] = { TOTAL_FOLDER ,LOG_FOLDER,DATA_FOLDER };
    // 使用mkdir函数创建文件夹
    for (i = 0; i < sizeof(dirname) / sizeof(dirname[0]); i++)
    {
        mkdir(dirname[i]);
    }
}

//创建文件
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
        printf("无法创建文件\n");
        return 1;
    }
    return 0;
}


//文件夹下文件数量
int get_file_num(char* dir)
{
    int txtCount = 0;
    // 构建目录搜索模式
    char searchPattern[FOLDER_NAME_LEN];
    snprintf(searchPattern, sizeof(searchPattern), "%s/*.txt", dir);
#ifdef _WIN32
    // Windows下的代码
    WIN32_FIND_DATA findData;
    HANDLE hFind = FindFirstFile(searchPattern, &findData);
    if (hFind == INVALID_HANDLE_VALUE) {
        //printf("dir can not find!\n");
        return 0;
    }
    do {
        if (findData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) {  // 仅处理普通文件
            txtCount++;
        }
    } while (FindNextFileA(hFind, &findData));
    FindClose(hFind);
#else
    // Linux下的代码
    FILE* pipe;
    char command[MAX_PATH_LENGTH + 8];  // "ls -1 <dir>"
    snprintf(command, sizeof(command), "ls -1 %s", searchPattern);

    // 执行ls命令获取文件列表
    pipe = popen(command, "r");
    if (pipe == NULL) {
        printf("无法打开目录！\n");
        return 1;
    }

    char fileName[MAX_PATH_LENGTH];
    while (fgets(fileName, sizeof(fileName), pipe) != NULL) {
        // 删除文件名末尾的换行符
        fileName[strcspn(fileName, "\n")] = '\0';

        if (currentIndex == targetFileIndex) {
            // 构建完整的文件路径
            char filePath[MAX_PATH_LENGTH];
            snprintf(filePath, sizeof(filePath), "%s/%s", directory, fileName);

            // 打开文件
            FILE* file = fopen(filePath, "r");
            if (file == NULL) {
                printf("无法打开文件：%s\n", filePath);
                pclose(pipe);
                return 1;
            }

            // 在这里处理或使用所打开的文件
            // ...

            // 关闭文件
            fclose(file);

            // 找到目标文件后退出循环
            break;
        }

        currentIndex++;
    }

    pclose(pipe);

#endif
    return txtCount;
}



