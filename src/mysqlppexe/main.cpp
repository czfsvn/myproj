#include <iostream>

#include "Log.h"
#include "Pool.h"
#include "Stock.h"



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

	MyConnectionPool::getMe().init(config);

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
	
	std::cout << "hello, main\n";
	return 1;
}