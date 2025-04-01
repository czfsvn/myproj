#include <iostream>

#include "Log.h"
#include "Pool.h"
#include "Stock.h"
#include "ApiTest.h"

namespace ns_test
{
	void test()
	{
		cncpp::MysqlConfig config;
		config.host = "localhost";
		config.user = "root";
		config.password = "123456";
		config.database = "mysql_cpp_data";

		

		{
			ScopedMySqlConn con;
			std::vector<Stock> vec = con->query<Stock>("select * from stock");
			for (Stock& st : vec)
			{
				st.print();
			}
		}
	}

}


int main()
{
	
	TRACE("test{}", 5);
	DEBUG("test{}", 5);
	INFO("test{}", 5);
	WARN("test{}", 5);
	ERR("test{}", 5);
	CRITICAL("test{}", 5);

	cncpp::MysqlConfig config;
	config.host = "localhost";
	config.user = "root";
	config.password = "123456";
	config.database = "mysql_cpp_data";

	MySqlConnectPool::getMe().init(config);

	ns_test::test();

	/*
2>Base.lib(Log.obj) : error LNK2005: "public: __cdecl std::basic_ostringstream<char,struct std::char_traits<char>,class std::allocator<char> >::basic_ostringstream<char,struct std::char_traits<char>,class std::allocator<char> >(void)" (??0?$basic_ostringstream@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QEAA@XZ) 已经在 mysqlpp.lib(mysqlpp.dll) 中定义

2>Base.lib(Log.obj) : error LNK2005: "public: void __cdecl std::basic_ostringstream<char,struct std::char_traits<char>,class std::allocator<char> >::`vbase destructor'(void)" (??_D?$basic_ostringstream@DU?$char_traits@D@std@@V?$allocator@D@2@@std@@QEAAXXZ) 已经在 mysqlpp.lib(mysqlpp.dll) 中定义
2>..\..\bin\Debug\x64\mysqlppexe_d.exe : fatal error LNK1169: 找到一个或多个多重定义的符号
	*/
	//std::ostringstream oss;
	//oss << 2;
	//std::string str =  oss.str();

	//MyConnectionPool::getMe().init(config);

	{
		ScopedMySqlConn con;
		std::vector<Stock> vec = con->query<Stock>("select * from stock");
		for ( Stock& st : vec)
		{
			st.print();
		}
	}

	{
		ScopedMySqlConn con;
		std::vector<Stock> vec = con->query<Stock>("select * from stock", "where item='Pickle Relish'");
		for (Stock& st : vec)
		{
			st.print();
		}
	}
	
	ns_api::main();

	std::cout << "hello, main\n";
	return 1;
}