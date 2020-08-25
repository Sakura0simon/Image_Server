#include <cstdio>
#include <iostream>
#include <string>
#include <mutex>
#include <jsoncpp/json/json.h>
#include <mysql/mysql.h>

#define MYSQL_HOST "127.0.0.1"
#define MYSQL_USER "root"
#define MYSQL_PSWD "Simon_sakura0"
#define MYSQL_NAME "image_server"

#define CHECK_RET(q) if((q)==false){return false;}

namespace image_server{
	static std::mutex g_mutex;
	static MYSQL * MysqlInit()
	{
	MYSQL *mysql=NULL;
	mysql=mysql_init(NULL);
	if(mysql==NULL)
	{
		printf("mysql init failed!!\n");
		return NULL;
	}
	if(mysql_real_connect(mysql, MYSQL_HOST, MYSQL_USER, MYSQL_PSWD, MYSQL_NAME, 0, NULL, 0)==NULL)
	{
		printf("connect mysql server failed: %s\n",mysql_error(mysql));
		return NULL;
	}
	if(mysql_set_character_set(mysql,"utf8")!=0)
	{
		printf("set character failed: %s\n",mysql_error(mysql));
		mysql_close(mysql);
		return NULL;
	}
	return mysql;
	}

	static void MysqlRelease(MYSQL *mysql)
	{
	if (mysql)
	{
		mysql_close(mysql);
	}
	return;
	}

	static bool MysqlQuery(MYSQL *mysql,const std::string sql)
	{
	int ret=mysql_query(mysql,sql.c_str());
	if(ret!=0)
	{
		printf("query sql:[%s] failed: [%s]\n",sql.c_str(),mysql_error(mysql));
		return false;
	}
	return true;
	}


	class TableImage
	{
	public:
		TableImage(MYSQL *mysql):_mysql(mysql){}

		bool Insert(const Json::Value &image)//插入图片元信息
		{
		#define IMAGE_INSERT "insert table_image values(null, '%s', '%d', '%s', '%s', '%s',now());"
		char sql[4096]={0};
		sprintf(sql,IMAGE_INSERT,image["name"].asCString(),image["fsize"].asInt(),image["fpath"].asCString(),image["furl"].asCString(),image["fmd5"].asCString());
		CHECK_RET(MysqlQuery(_mysql,sql));
		return true;	
		}

		bool Delete(const int image_id)//删除图片元信息
		{
		#define IMAGE_DELETE "delete from table_image where id=%d;"
		char sql[4096]={0};
		sprintf(sql,IMAGE_DELETE,image_id);
		CHECK_RET(MysqlQuery(_mysql,sql));
		return true;
		}

		bool Update(const int image_id,const Json::Value &image)//修改图片元信息
		{
		#define IMAGE_UPDATE "update table_image set name='%s' where id=%d;"
		char sql[4096]={0};
		sprintf(sql,IMAGE_UPDATE,image["name"].asCString(),image_id);
		CHECK_RET(MysqlQuery(_mysql,sql));
		return true;
		}

		bool GetAll(Json::Value *images)//获取所有图片元信息
		{
		#define IMAGE_GETALL "select * from table_image;"
		g_mutex.lock();
		CHECK_RET(MysqlQuery(_mysql,IMAGE_GETALL));
		MYSQL_RES *res =mysql_store_result(_mysql);
		g_mutex.unlock();
		if(res == NULL)
		{
			printf("store result failed: %s\n",mysql_error(_mysql));
			return false;
		}
		int num_row = mysql_num_rows(res);
		for(int i=0; i< num_row; i++)
		{
			MYSQL_ROW row=mysql_fetch_row(res);
			Json::Value image;
			image["id"]=std::stoi(row[0]);
			image["name"]=row[1];
			image["fsize"]=std::stoi(row[2]);
			image["fpath"]=row[3];
			image["furl"]=row[4];
			image["fmd5"]=row[5];
			image["ultime"]=row[6];
			images->append(image);//将每一个图片对象添加到Json::Value数组中
		}
		mysql_free_result(res);
		return true;
		}

		bool GetOne(const int image_id,Json::Value *image)//获取单个图片元信息
		{
		#define IMAGE_GETONE "select * from table_image where id=%d;"
		g_mutex.lock();
		char sql[4096]={0};
		sprintf(sql,IMAGE_GETONE,image_id);
		CHECK_RET(MysqlQuery(_mysql,sql));
		MYSQL_RES *res=mysql_store_result(_mysql);
		g_mutex.unlock();
		if(res == NULL)
		{
			printf("get one image failed:%s\n",mysql_error(_mysql));
			return false;
		}
		int num_row=mysql_num_rows(res);
		if(num_row!=1)
		{
			printf("get one image result error\n");
			mysql_free_result(res);
			return false;
		}
		for(int i=0;i<num_row;i++)
		{
			MYSQL_ROW row=mysql_fetch_row(res);
			(*image)["id"]=std::stoi(row[0]);
			(*image)["name"]=row[1];
			(*image)["fsize"]=std::stoi(row[2]);
			(*image)["fpath"]=row[3];
			(*image)["furl"]=row[4];
			(*image)["fmd5"]=row[5];
			(*image)["ultime"]=row[6];
		}
		mysql_free_result(res);
		return true;
		}
	private:
		MYSQL *_mysql;
	};

}
