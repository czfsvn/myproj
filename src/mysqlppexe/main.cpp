#include <iostream>

#include "ApiTest.h"
#include "Log.h"
#include "Pool.h"
#include "Stock.h"


namespace ns_test
{
    void test()
    {

        {
            ScopedMySqlConn    con;
            std::vector<Stock> vec = con->query<Stock>("select * from stock");
            for (Stock& st : vec)
            {
                st.dump();
            }
        }

        {
            ScopedMySqlConn    con;
            std::vector<Stock> vec = con->query<Stock>("select * from stock");
            for (Stock& st : vec)
            {
                st.dump();
            }
        }

        {
            ScopedMySqlConn    con;
            std::vector<Stock> vec =
                con->query<Stock>("select * from stock", "where item='Pickle Relish'");
            for (Stock& st : vec)
            {
                st.dump();
            }
        }
    }

}  // namespace ns_test

int main()
{
    TRACE("test{}", 5);
    DEBUG("test{}", 5);
    INFO("test{}", 5);
    WARN("test{}", 5);
    ERR("test{}", 5);
    CRITICAL("test{}", 5);

    cncpp::MysqlConfig config;
    config.host     = "localhost";
    config.user     = "root";
    config.password = "123456";
    config.database = "mysql_cpp_data";

    MySqlConnectPool::getMe().init(config);

    //ns_test::test();

    ns_api::main();

    std::cout << "hello, main\n";
    return 1;
}