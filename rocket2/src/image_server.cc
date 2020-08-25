#include "db.hpp"
#include "httplib.h"
#include <openssl/md5.h>

#define WWW_ROOT "./www"
#define IMAGE_PATH "/images/"

image_server::TableImage *image_table;

bool WriteFile(const std::string &filename,const std::string body)
{
std::ofstream ofs(filename, std::ios::binary);//二进制文件
if(ofs.is_open()==false)
{
 printf("open file %s failed!\n",filename.c_str());
 return false;
}
ofs.write(body.c_str(),body.size());
if(ofs.good()==false)
{	
	printf("write file body failed!\n");
	ofs.close();
	return false;
}
ofs.close();
return true;
}

void MD5_CALC(const std::string &body,std::string *md5)
{
unsigned char buf[16] = {0};
MD5((const unsigned char *)body.c_str(),body.size(),buf);
for(int i=0;i<16;i++)
{
	char tmp[3]={0};
	sprintf(tmp,"%02x",buf[i]);
	*md5 += tmp;
}
}

///////////////////////////////////////////////////////////////////////////
void db_test()
{

//实例化一个数据库表访问对象，实现数据的增删改查操作进行局部的功能测试
MYSQL *mysql = image_server::MysqlInit();
image_server::TableImage table_image(mysql);

//插入
Json::Value image;
//image["name"]="阴天";
//image["fsize"]=4096;
//image["fpath"]="./www/test.png";
//image["furl"]="/www/test.png";
//image["fmd5"]="31das32d1a23das32";
//table_image.Insert(image);

//修改
//image["name"]="慕白";
//table_image.Update(5,image);

//删除
table_image.Delete(5);

//查询所有图片元信息
//table_image.GetAll(&image);
//Json::StyledWriter writer;
//std::cout<<writer.write(image)<<std::endl;

//查询单个图片元信息
//table_image.GetOne(1, &image);
//Json::StyledWriter writer;
//std::cout<<writer.write(image)<<std::endl;

//image_server::MysqlRelease(mysql);
}
////////////////////////////////////////////////////////////////////////

void GetAllImage(const httplib::Request &req, httplib::Response &rsp)
{
//从数据库中获取到所有的元信息
Json::Value images;
bool ret = image_table->GetAll(&images);
if(ret == false)
{
	printf("Failed to get all data from database\n");
	return;
}
//序列化成字符串，放入响应文件中
Json::FastWriter writer;
//rsp.body = writer.write(images);//body是个string对象，直接对对象赋值
//设置响应正文类型 application/json
rsp.set_content(writer.write(images),"application/json");

//rsp.set_header("Content-Length",std::to_string(rsp.body.size()));
//rsp.set_content(writer.write(images),"application/json");//可以代替body和set_header两步
//设置响应状态码
rsp.status = 200;
printf("获取所有图片元信息请求处理成功\n");
return ; 
}


void GetOneImage(const httplib::Request &req, httplib::Response &rsp)
{
//path =/image/(\d+)
//从请求中获取所需图片元信息的id
//req.matches存放匹配字符串
int image_id = std::stoi(req.matches[1]);
//从数据库获取图片元信息数据
Json::Value image;
bool ret = image_table->GetOne(image_id,&image);
if(ret == false)
{
	printf("Failed to get one data from database\n");
	rsp.status = 500;//表示服务器错误
	return;
}
//进行数据序列化，填充到响应正文中
//设置头部字段
Json::FastWriter writer;
rsp.set_content(writer .write(image),"application/json");
//服务器默认自动设置状态码为200；
return ; 
}


void DeleteImage(const httplib::Request &req, httplib::Response &rsp)
{
//获取要删除的图片ID
int image_id = std::stoi(req.matches[1]);
//从数据库获取图片元信息，实际路径，删除图片文件
Json::Value image;
bool ret = image_table->GetOne(image_id,&image);
if(ret == false)
{
	printf("Failed to get image meta information when deleting picture\n");
	rsp.status = 500;
	return;
}
std::string real_path = image["fpath"].asString();
unlink(real_path.c_str());//删除文件
//从数据库中删除图片元信息
ret = image_table->Delete(image_id);
if(ret == false)
{
	printf("Failed to delete image meta information from database\n");
	rsp.status = 500;
	return;
}
printf("删除图片元信息成功\n");
return ; 
}


void UpdateImage(const httplib::Request &req, httplib::Response &rsp)
{
//从请求中过去要修改的图片id，以及修改后额图片数据
int image_id = std::stoi(req.matches[1]);
//要修改的信息在请求的正文中，是一个json字符串
Json::Value image;
Json::Reader reader;
bool ret = reader.parse(req.body,image);
if(ret = false)
{
	printf("The modified picture information is incorrect\n");
	rsp.status = 400;
	return;
}
//从数据库中修改指定图片的元信息
ret = image_table->Update(image_id, image);
if(ret == false)
{
	printf("Failed to modify image information from database\n");
	rsp.status = 500;
	return;
}
return ; 
}

//POST方法
void AppendImage(const httplib::Request &req, httplib::Response &rsp)
{
bool ret = req.has_file("imagefile");
if(ret == false)
{
	printf("The name of the uploaded file was not found\n");
	rsp.status = 400;
	return ;
}
auto file= req.get_file_value("imagefile");//获取文件信息
//file.filename文件名 file.content文件数据
//计算文件大小  计算文件md5   计算文件实际存储路径名   计算url
int fsize = file.content.size();
std::string md5_str;
MD5_CALC(file.content,&md5_str);
std::string image_url = IMAGE_PATH + file.filename;// /image/test.png
std::string real_path = WWW_ROOT + image_url;//   .www/image/test.png

//将图片元信息插入数据库
Json::Value image;
image["name"]=file.filename;
image["fsize"]=(Json::Value::Int64)file.content.size();
image["fpath"]=real_path;
image["furl"]=image_url;
image["fmd5"]=md5_str;

ret = image_table->Insert(image); 
if(ret == false)
{
	printf("Failed to append image information into database\n");
	rsp.status = 500;
	return;
}
WriteFile(real_path,file.content);

rsp.set_redirect("/");//重定向
return ; 
}


int main(int argc,char *argv[])
{

MYSQL *mysql = image_server::MysqlInit();
image_table = new image_server::TableImage(mysql); 
httplib::Server srv;
srv.set_base_dir(WWW_ROOT);//设置静态文件资源根目录，当收到文件请求httplib自动响应
//注册请求路由信息
//获取图片元信息请求
srv.Get("/image",GetAllImage);
//获取指定图片数据请求-------httplib会自动处理静态文件请求
//获取指定图片元信息请求
//正则表达式/image/(\d+)----匹配image/后紧跟1个或者多个数字字符；(\d+)表示捕捉该数据
srv.Get(R"(/image/(\d+))",GetOneImage);
//删除图片请求
srv.Delete(R"(/image/(\d+))",DeleteImage);
//上传图片请求
srv.Post("/image",AppendImage);
//修改图片元信息请求
srv.Put(R"(/image/(\d+))",UpdateImage);

srv.listen("0.0.0.0",9000);
image_server::MysqlRelease(mysql);

//db_test();
return 0;
}