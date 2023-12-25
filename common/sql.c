//#include "sql.h"
//
//MYSQL mysql;
//
//int mysql_test(void)
//{
//
//	sql_init();
//
//	char sql[] = "select * from test1;";
//	sql_query(sql, sizeof(sql));
//	//char sql[] = "insert into test1 (id, type) values (3, 198);";
//	//sql_insert(sql, sizeof(sql));
//	return 0;
//}
//
///*
//作用：初始化mysql并连接上远程数据库
//参数：
//返回值：成功则返回0
//*/
//int sql_init(void)
//{
//	//初始化mysql句柄
//	mysql_init(&mysql);
//	/*
//	连接mysql
//	host主机IP:localhost
//	user用户名:root
//	password密码:123456
//	database数据库名:test
//	port端口:3306
//	*/
//	if (mysql_real_connect(&mysql, "localhost", "root", "123456", "test", 3306, NULL, 0) == NULL)
//	{
//		printf("mysql connect fail:%d\n", mysql_errno(&mysql));
//	}
//
//	//sql语句，创建表格
//	char sql[] = "create table if not exists test1 \
//		(\
//		id int comment '序号',\
//		type varchar(10) comment '类型'\
//		);";
//	mysql_real_query(&mysql, sql, sizeof(sql));
//
//	return 0;
//}
//
///*
//作用：通过sql语句查询数据库的数据
//参数：mysql句柄，sql语句，sql语句字符串长度
//返回值：成功则返回0
//*/
//int sql_query(char* sql, int len)
//{
//	//结果集，行结果，字段集
//	MYSQL_RES* mysqlRes;
//	MYSQL_ROW  mysqlRow;
//	MYSQL_FIELD* mysqlField;
//	//列数，行数
//	int column;
//	int row;
//	int i;
//
//	//查询
//	mysql_real_query(&mysql, sql, len);
//	//存储查询到的数据
//	mysqlRes = mysql_store_result(&mysql);
//	if (mysqlRes == NULL)
//	{
//		printf("mysql query fail:%d\n", mysql_errno(&mysql));
//	}
//	//获取字段数(列数)，行数
//	row = mysql_num_rows(mysqlRes);
//	column = mysql_num_fields(mysqlRes);	
//	printf("row:%d\tcolumn:%d\n", row, column);
//
//	//获取字段名称并打印
//	mysqlField = mysql_fetch_field(mysqlRes);
//	for (i = 0; i < column; i++)
//	{
//		printf("%s\t", mysqlField[i].name);
//	}
//	printf("\n");
//
//	//获取数据并打印
//	while (mysqlRow = mysql_fetch_row(mysqlRes))
//	{
//		for (i = 0; i < column; i++)
//		{
//			printf("%s\t", mysqlRow[i]);
//		}
//		printf("\n");
//	}
//	//释放存储资源
//	mysql_free_result(mysqlRes);
//	return 0;
//}
//
///*
//作用：通过sql语句向数据库中插入数据
//参数：mysql句柄，sql语句，sql语句字符串长度
//返回值：成功则返回0
//*/
//int sql_insert(char* sql, int len)
//{
//	int ret;
//	ret = mysql_real_query(&mysql, sql, len);
//	if (ret != NULL)
//	{
//		printf("mysql insert fail:%d\n", mysql_errno(&mysql));
//	}
//	return 0;
//}
//
///*
//作用：释放存储资源，关闭连接
//参数：mysql句柄，查询到的mysqlResult
//返回值：成功则返回0
//*/
//int sql_free(MYSQL * mysql, MYSQL_RES * mysqlRes)
//{
//	mysql_free_result(mysqlRes);
//	mysql_close(mysql);
//	return 0;
//}