#ifndef _SQL_H_
#define _SQL_H_

#include <winsock2.h>
#include <stdio.h>
#include <mysql.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libmysql.lib")


int mysql_test(void);
int sql_init(void);
int sql_query(char* sql, int len);
int sql_insert(char* sql, int len);
int sql_free(MYSQL* mysql, MYSQL_RES* mysqlRes);

extern MYSQL mysql;

#endif