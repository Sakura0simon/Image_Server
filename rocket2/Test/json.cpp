#include<iostream>
#include<string>
#include<jsoncpp/json/json.h>

int main(int arcg,char *argv[])
{
	std::string name="simon";
	int age=18;
	std::string sex="男";
	float chinese=100.0;
	float english=98.5;
	float math=96.1;
	Json::Value val;//定义Json::Value对象，将数据都添加到其中
	val["姓名"]=name;//这是对[]运算符重载，数据对象在Json::Value中存储，以键值对的形式存在
	val["年龄"]=age;
	val["性别"]=sex;
	val["成绩"].append(chinese);//使用append会将你的数据当数组的元素添加
	val["成绩"].append(english);
	val["成绩"].append(math);
	//数据序列化
	Json::StyledWriter writer;
	std::string json_str=writer.write(val);
	std::cout<<json_str<<std::endl;
	
	std::string str=R"({"name":"张三","age":28,"sex":"男","score":[96,85,94]})";
	Json::Reader reader;
	Json::Value root;
	bool ret = reader.parse(str,root);//将字符串中的各个数据对象解析出来
	if(ret==false)
	{
		return -1;
	}
	std::cout<<"姓名:"<<root["name"].asString()<<std::endl;
	std::cout<<"性别:"<<root["sex"].asCString()<<std::endl;
	std::cout<<"年龄:"<<root["age"].asInt()<<std::endl;
	int num = root["score"].size();
	for(int i=0;i<num;i++)
	{
		std::cout<<"成绩："<<root["score"][i].asFloat()<<std::endl;
	}
	return 0;
}
