#include <iostream>

#include "Log.h"
#include "Pool.h"

// ���Ӳ���
const char* db_host = "localhost";
const char* db_user = "root";
const char* db_pass = "123456";
const char* db_name = "myapp_db";  // ��ʼ��ʱ���ܲ����ڣ���������

int main()
{
	TRACE("test{}", 5);
	DEBUG("test{}", 5);
	INFO("test{}", 5);
	WARN("test{}", 5);
	ERR("test{}", 5);
	CRITICAL("test{}", 5);

	MyConnectionPool::getMe();
	std::cout << "hello, main\n";
	return 1;
}