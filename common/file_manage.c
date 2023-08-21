#include "file_manage.h"


//file_info_t log_file;
//file_info_t data_file;
//file_info_t sche_file;



file_config_t file_config[] = {
    {DATA_FOLDER,"data",},
    {LOG_FOLDER,"log",},
    {SCHE_FOLDER,"sche",},
};


void file_init()
{
    int i = 0;
    create_folder();
    for (i = 0; i < sizeof(file_config) / sizeof(file_config[0]); i++)
    {
        create_file(&file_config[i].file, file_config[i].directory, file_config[i].name);
        if (i == 0)
        {
            create_map(&file_config[i].file, 10);
        }
        else
        {
            create_map(&file_config[i].file, 2);
        }
    }
}


//文件夹创建
void create_folder()
{
    int i = 0;
    char dirname[][FOLDER_NAME_LEN] = { TOTAL_FOLDER ,LOG_FOLDER,DATA_FOLDER,SCHE_FOLDER };
    // 使用mkdir函数创建文件夹
    for (i = 0; i < sizeof(dirname) / sizeof(dirname[0]); i++)
    {
        mkdir(dirname[i]);
    }
}

//创建文件
void create_file(file_info_t* file,char* dir,char* name)
{
    char n[100];
    char n1[100];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    strftime(n, sizeof(n), "%Y%m%d_%H-%M-%S", tm);
    sprintf(n1, "%s\\%d_%s_%s.txt", dir, MY_INDEX, n, name);
    file->file = fopen(n1, "w+");
    if (file->file == NULL) {
        printf("无法创建文件\n");
        return 1;
    }
    strcpy(file->name, n1);
    fclose(file->file);
    pthread_mutex_init(&file->lock, NULL);
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

//内存映射
void create_map(file_info_t* file,int size)
{
    file->fileHandle = CreateFileA(file->name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file->fileHandle == INVALID_HANDLE_VALUE) {
        perror("无法打开文件");
        printf("%s\n", file->name);
        return 1;
    }
    // 设置文件大小
    DWORD fileSizeHigh = 0; // 高32位文件大小
    DWORD fileSizeLow = size * 1024 * 1024; // 低32位文件大小
    DWORD newFilePointer = SetFilePointer(file->fileHandle, fileSizeLow, &fileSizeHigh, FILE_BEGIN);
    file->size = fileSizeLow;
    if (newFilePointer == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
        perror("设置文件指针位置失败");
        CloseHandle(file->fileHandle);
        return 1;
    }
    if (!SetEndOfFile(file->fileHandle)) {
        perror("设置文件大小失败");
        CloseHandle(file->fileHandle);
        return 1;
    }
    char data = 'A';
    DWORD bytesWritten;
    if (!WriteFile(file->fileHandle, &data, sizeof(data), &bytesWritten, NULL)) {
        perror("写入文件失败");
        CloseHandle(file->fileHandle);
        return 1;
    }
    // 创建文件的内存映射
    file->mappingHandle = CreateFileMapping(file->fileHandle, NULL, PAGE_READWRITE, 0, 0, NULL);
    if (file->mappingHandle == NULL) {
        perror("无法创建内存映射");
        CloseHandle(file->fileHandle);
        return 1;
    }
    // 将文件的内存映射映射到进程地址空间
    file->mappedData = MapViewOfFile(file->mappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (file->mappedData == NULL) {
        perror("无法映射文件到内存");
        CloseHandle(file->mappingHandle);
        CloseHandle(file->fileHandle);
        return 1;
    }

    return 0;
}


void tofile(int i, char* s, ...)
{
    va_list args;
    va_start(args, s);
    pthread_mutex_lock(&file_config[i].file.lock);
    int written = vsnprintf(file_config[i].file.mappedData + file_config[i].file.ptr, file_config[i].file.size - file_config[i].file.ptr, s, args);
    if (written >= 0) {
        file_config[i].file.ptr += written;
    }
    else {
        printf("写入失败：%d\n", i);
    }
    pthread_mutex_unlock(&file_config[i].file.lock);
}



void todata(char* data, int len)
{
    pthread_mutex_lock(&file_config[0].file.lock);
    if (file_config[0].file.ptr > 1024 * 1024 * 9.5)
    {
        printf("***************data full*****************\n");
        pthread_mutex_unlock(&file_config[0].file.lock);
        return;
    }
    memcpy(file_config[0].file.mappedData + file_config[0].file.ptr, data, len);
    file_config[0].file.ptr += len;
    pthread_mutex_unlock(&file_config[0].file.lock);
    //fwrite(data, sizeof(char), len, file_config[0].file.file);
}



void for_test()
{
    HANDLE fileHandle;
    HANDLE mappingHandle;
    LPVOID mappedData;

    // 打开文件
    fileHandle = CreateFileA("C:\\Digital prototype\\log\\20230711_17-07-13_log.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        perror("无法打开文件");
        return 1;
    }

    // 设置文件大小为500MB
    DWORD fileSizeHigh = 0; // 高32位文件大小
    DWORD fileSizeLow = 2 * 1024 * 1024; // 低32位文件大小
    DWORD newFilePointer = SetFilePointer(fileHandle, fileSizeLow, &fileSizeHigh, FILE_BEGIN);
    if (newFilePointer == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
        perror("设置文件指针位置失败");
        CloseHandle(fileHandle);
        return 1;
    }
    if (!SetEndOfFile(fileHandle)) {
        perror("设置文件大小失败");
        CloseHandle(fileHandle);
        return 1;
    }

    char data = 'A';
    DWORD bytesWritten;
    if (!WriteFile(fileHandle, &data, sizeof(data), &bytesWritten, NULL)) {
        perror("写入文件失败");
        CloseHandle(fileHandle);
        return 1;
    }

    // 创建文件的内存映射
    mappingHandle = CreateFileMapping(fileHandle, NULL, PAGE_READWRITE, 0, 0, NULL);
    if (mappingHandle == NULL) {
        perror("无法创建内存映射");
        CloseHandle(fileHandle);
        return 1;
    }

    // 将文件的内存映射映射到进程地址空间
    mappedData = MapViewOfFile(mappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (mappedData == NULL) {
        perror("无法映射文件到内存");
        CloseHandle(mappingHandle);
        CloseHandle(fileHandle);
        return 1;
    }

    // 访问和修改文件内容
    printf("文件内容：%s\n", (char*)mappedData);
    ((char*)mappedData)[0] = 'H';

    // 解除内存映射
    UnmapViewOfFile(mappedData);
    CloseHandle(mappingHandle);
    CloseHandle(fileHandle);

    return 0;
}


