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
//���ã���ʼ��mysql��������Զ�����ݿ�
//������
//����ֵ���ɹ��򷵻�0
//*/
//int sql_init(void)
//{
//	//��ʼ��mysql���
//	mysql_init(&mysql);
//	/*
//	����mysql
//	host����IP:localhost
//	user�û���:root
//	password����:123456
//	database���ݿ���:test
//	port�˿�:3306
//	*/
//	if (mysql_real_connect(&mysql, "localhost", "root", "123456", "test", 3306, NULL, 0) == NULL)
//	{
//		printf("mysql connect fail:%d\n", mysql_errno(&mysql));
//	}
//
//	//sql��䣬�������
//	char sql[] = "create table if not exists test1 \
//		(\
//		id int comment '���',\
//		type varchar(10) comment '����'\
//		);";
//	mysql_real_query(&mysql, sql, sizeof(sql));
//
//	return 0;
//}
//
///*
//���ã�ͨ��sql����ѯ���ݿ������
//������mysql�����sql��䣬sql����ַ�������
//����ֵ���ɹ��򷵻�0
//*/
//int sql_query(char* sql, int len)
//{
//	//��������н�����ֶμ�
//	MYSQL_RES* mysqlRes;
//	MYSQL_ROW  mysqlRow;
//	MYSQL_FIELD* mysqlField;
//	//����������
//	int column;
//	int row;
//	int i;
//
//	//��ѯ
//	mysql_real_query(&mysql, sql, len);
//	//�洢��ѯ��������
//	mysqlRes = mysql_store_result(&mysql);
//	if (mysqlRes == NULL)
//	{
//		printf("mysql query fail:%d\n", mysql_errno(&mysql));
//	}
//	//��ȡ�ֶ���(����)������
//	row = mysql_num_rows(mysqlRes);
//	column = mysql_num_fields(mysqlRes);	
//	printf("row:%d\tcolumn:%d\n", row, column);
//
//	//��ȡ�ֶ����Ʋ���ӡ
//	mysqlField = mysql_fetch_field(mysqlRes);
//	for (i = 0; i < column; i++)
//	{
//		printf("%s\t", mysqlField[i].name);
//	}
//	printf("\n");
//
//	//��ȡ���ݲ���ӡ
//	while (mysqlRow = mysql_fetch_row(mysqlRes))
//	{
//		for (i = 0; i < column; i++)
//		{
//			printf("%s\t", mysqlRow[i]);
//		}
//		printf("\n");
//	}
//	//�ͷŴ洢��Դ
//	mysql_free_result(mysqlRes);
//	return 0;
//}
//
///*
//���ã�ͨ��sql��������ݿ��в�������
//������mysql�����sql��䣬sql����ַ�������
//����ֵ���ɹ��򷵻�0
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
//���ã��ͷŴ洢��Դ���ر�����
//������mysql�������ѯ����mysqlResult
//����ֵ���ɹ��򷵻�0
//*/
//int sql_free(MYSQL * mysql, MYSQL_RES * mysqlRes)
//{
//	mysql_free_result(mysqlRes);
//	mysql_close(mysql);
//	return 0;
//}