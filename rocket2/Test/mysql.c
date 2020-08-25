/*描述：编写简单的mysql接口使用用例，学习接口的使用*/

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

int main(int argc,char *argv[])
{
	//1.初始化句柄
	MYSQL *mysql = mysql_init(NULL);
	if(mysql==NULL)
	{
		printf("mysql init error\n");
		return -1;
	} 
	//2.连接服务器
	//mysql_real_connect(句柄,主机,地址,用户名,密码,库名称,端口号,套接字,标志位)
	if(mysql_real_connect(mysql, "127.0.0.1", "root", "Simon_sakura0", "image_server", 0, NULL, 0)==NULL)
	{
		printf("connect mysql server failed: %S\n", mysql_error(mysql));
		mysql_close(mysql);
		return -1;	
	}
	//3.选择数据库
	//if(mysql_select_db(mysql,"image_server")!=0)
	//{
	//	printf("select db failed! : %s\n",mysql_error(mysql));
	//	close(mysql);
	//	return -1;
	//}

	//4.设置字符集
if(mysql_set_character_set(mysql,"utf8")!=0)
	{
		printf("set character failed! : %s\n",mysql_error(mysql));
		mysql_close(mysql);
		return -1;
	}

	//5.执行语句---执行语句，获取结果集，获取条数与列数，遍历结果，释放结果集
	//(1) 增添数据
	//char *sql_insert = "insert table_image values(null, '枫叶', 2048,'./a.jpg', '/image/a.jpg', 'dsadasd1a2s3d12as3', now());";	

	//(2) 更新数据
	//char *sql_update = "update table_image set name='大雾' where id=4;";

	//(3) 删除数据
	//char *sql_delete = "delete from table_image where id=4;";

	//(4) 查找数据
	char *sql_select = "select * from table_image;";
	if(mysql_query(mysql,sql_select)!=0)
	{
		printf("query sql failed : %s\n",mysql_error(mysql));
		mysql_close(mysql);	
		return -1;
	}
	MYSQL_RES *res = mysql_store_result(mysql);//获取结果集
	if(res == NULL)
	{
		printf("store result failed: %s\n", mysql_error(mysql));
		mysql_close(mysql);
		return -1;
	}
	int num_row = mysql_num_rows(res);//获取结果条数
	int num_col = mysql_num_fields(res);//获取结果列数
	int i,j;
	for(i =0; i< num_row; i++)
	{
		MYSQL_ROW row = mysql_fetch_row(res);//遍历结果集，读位置自动向后偏移
		for(j=0;j<num_col;j++)
		{
			printf("%s \t",row[j]);
		}
		printf("\n");
	}
	mysql_free_result(res);//释放结果集
	//6.关闭数据库
	mysql_close(mysql);
	return 0;
}