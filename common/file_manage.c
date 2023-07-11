#include "file_manage.h"


file_info_t log_file;
file_info_t data_file;


void file_init()
{
    create_folder();
    create_file(&data_file, DATA_FOLDER, "data");
    create_file(&log_file, LOG_FOLDER, "log");
    create_map(&data_file, 2);
    create_map(&log_file, 2);
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
void create_file(file_info_t* file,char* dir,char* name)
{
    char n[100];
    char n1[100];
    time_t t = time(NULL);
    struct tm* tm = localtime(&t);
    strftime(n, sizeof(n), "%Y%m%d_%H-%M-%S", tm);
    sprintf(n1, "%s\\%s_%s.txt", dir, n, name);
    file->file = fopen(n1, "w+");
    if (file->file == NULL) {
        printf("�޷������ļ�\n");
        return 1;
    }
    strcpy(file->name, n1);
    fclose(file->file);
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

//�ڴ�ӳ��
void create_map(file_info_t* file,int size)
{
    file->fileHandle = CreateFileA(file->name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file->fileHandle == INVALID_HANDLE_VALUE) {
        perror("�޷����ļ�");
        printf("%s\n", file->name);
        return 1;
    }
    // �����ļ���С
    DWORD fileSizeHigh = 0; // ��32λ�ļ���С
    DWORD fileSizeLow = size * 1024 * 1024; // ��32λ�ļ���С
    DWORD newFilePointer = SetFilePointer(file->fileHandle, fileSizeLow, &fileSizeHigh, FILE_BEGIN);
    file->size = fileSizeLow;
    if (newFilePointer == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
        perror("�����ļ�ָ��λ��ʧ��");
        CloseHandle(file->fileHandle);
        return 1;
    }
    if (!SetEndOfFile(file->fileHandle)) {
        perror("�����ļ���Сʧ��");
        CloseHandle(file->fileHandle);
        return 1;
    }
    char data = 'A';
    DWORD bytesWritten;
    if (!WriteFile(file->fileHandle, &data, sizeof(data), &bytesWritten, NULL)) {
        perror("д���ļ�ʧ��");
        CloseHandle(file->fileHandle);
        return 1;
    }
    // �����ļ����ڴ�ӳ��
    file->mappingHandle = CreateFileMapping(file->fileHandle, NULL, PAGE_READWRITE, 0, 0, NULL);
    if (file->mappingHandle == NULL) {
        perror("�޷������ڴ�ӳ��");
        CloseHandle(file->fileHandle);
        return 1;
    }
    // ���ļ����ڴ�ӳ��ӳ�䵽���̵�ַ�ռ�
    file->mappedData = MapViewOfFile(file->mappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (file->mappedData == NULL) {
        perror("�޷�ӳ���ļ����ڴ�");
        CloseHandle(file->mappingHandle);
        CloseHandle(file->fileHandle);
        return 1;
    }

    return 0;
}


void tolog(char* s,...)
{
    va_list args;
    va_start(args, s);
    int written = vsnprintf(log_file.mappedData + log_file.ptr, log_file.size - log_file.ptr, s, args);
    if (written >= 0) {
        log_file.ptr += written;
    }
    else {
        perror("д����־�ļ�ʧ��");
    }
}



void for_test()
{
    HANDLE fileHandle;
    HANDLE mappingHandle;
    LPVOID mappedData;

    // ���ļ�
    fileHandle = CreateFileA("C:\\Digital prototype\\log\\20230711_17-07-13_log.txt", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (fileHandle == INVALID_HANDLE_VALUE) {
        perror("�޷����ļ�");
        return 1;
    }

    // �����ļ���СΪ500MB
    DWORD fileSizeHigh = 0; // ��32λ�ļ���С
    DWORD fileSizeLow = 2 * 1024 * 1024; // ��32λ�ļ���С
    DWORD newFilePointer = SetFilePointer(fileHandle, fileSizeLow, &fileSizeHigh, FILE_BEGIN);
    if (newFilePointer == INVALID_SET_FILE_POINTER && GetLastError() != NO_ERROR) {
        perror("�����ļ�ָ��λ��ʧ��");
        CloseHandle(fileHandle);
        return 1;
    }
    if (!SetEndOfFile(fileHandle)) {
        perror("�����ļ���Сʧ��");
        CloseHandle(fileHandle);
        return 1;
    }

    char data = 'A';
    DWORD bytesWritten;
    if (!WriteFile(fileHandle, &data, sizeof(data), &bytesWritten, NULL)) {
        perror("д���ļ�ʧ��");
        CloseHandle(fileHandle);
        return 1;
    }

    // �����ļ����ڴ�ӳ��
    mappingHandle = CreateFileMapping(fileHandle, NULL, PAGE_READWRITE, 0, 0, NULL);
    if (mappingHandle == NULL) {
        perror("�޷������ڴ�ӳ��");
        CloseHandle(fileHandle);
        return 1;
    }

    // ���ļ����ڴ�ӳ��ӳ�䵽���̵�ַ�ռ�
    mappedData = MapViewOfFile(mappingHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (mappedData == NULL) {
        perror("�޷�ӳ���ļ����ڴ�");
        CloseHandle(mappingHandle);
        CloseHandle(fileHandle);
        return 1;
    }

    // ���ʺ��޸��ļ�����
    printf("�ļ����ݣ�%s\n", (char*)mappedData);
    ((char*)mappedData)[0] = 'H';

    // ����ڴ�ӳ��
    UnmapViewOfFile(mappedData);
    CloseHandle(mappingHandle);
    CloseHandle(fileHandle);

    return 0;
}