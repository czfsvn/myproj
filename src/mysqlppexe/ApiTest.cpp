#include "ApiTest.h"
#include <iostream>

using namespace std;

#include "Pool.h"
#include "Stock.h"


namespace ns_query
{
    void print(const mysqlpp::Row& data)
    {
        Stock st(data);
        st.print();
    }

    void print(mysqlpp::StoreQueryResult& res)
    {
        for (auto iter = res.begin(); iter != res.end(); ++iter)
        {
            print(*iter);
        }
    }

    void test_storein()
    {
#if 1
        ScopedMySqlConn con;
        // mysqlpp::Query query = con->getConn().query("select item,description from stock");
        mysqlpp::Query query = con->getConn().query("select * from stock");
        vector<Stock>  res;
        query.storein(res);  // 需要满足 Stock 构造函数与 row 的转换

        for (const auto& item : res)
        {
            item.print();
        }
#endif
    }

    void test_quote1()
    {
        ScopedMySqlConn con;
        mysqlpp::Query  query = con->getConn().query("select * from stock where item = %0q");
        query.parse();

        // Retrieve an item added by resetdb; it won't be there if
        // tquery* or ssqls3 is run since resetdb.
        mysqlpp::StoreQueryResult res = query.store("Red LED, 5mm, 3000mcd");
        if (res.empty())
        {
            throw mysqlpp::BadQuery("UTF-8 bratwurst item not found in "
                                    "table, run resetdb");
        }

        print(res);

        res = query.store("C Battery, 4-pack");
        if (res.empty())
        {
            throw mysqlpp::BadQuery("UTF-8 bratwurst item not found in "
                                    "table, run resetdb");
        }

        print(res);
    }

    void test_quote2()
    {
        ScopedMySqlConn con;
        mysqlpp::Query  query = con->getConn().query("select * from stock where item = %0q");
        query.parse();

        mysqlpp::SQLQueryParms sqp;
        sqp << "Red LED, 5mm, 3000mcd";

        mysqlpp::StoreQueryResult res = query.store(sqp);
        if (res.empty())
        {
            throw mysqlpp::BadQuery("UTF-8 bratwurst item not found in "
                                    "table, run resetdb");
        }

        print(res);

        sqp.clear();
        sqp << "C Battery, 4-pack";

        res = query.store(sqp);
        if (res.empty())
        {
            throw mysqlpp::BadQuery("UTF-8 bratwurst item not found in "
                                    "table, run resetdb");
        }

        print(res);
    }

    void test_template()
    {
        ScopedMySqlConn con;
        mysqlpp::Query  query =
            con->getConn().query("select * from stock where num < %0:value and price >%1:pri");
        query.parse();
        query.template_defaults["value"] = 200;
        query.template_defaults["pri"]   = 10;

        CRITICAL("query: {}", query.str());

        mysqlpp::StoreQueryResult res = query.store();
        if (res.empty())
        {
            throw mysqlpp::BadQuery("UTF-8 bratwurst item not found in "
                                    "table, run resetdb");
        }

        print(res);
    }
}  // namespace ns_query

namespace ns_api
{
    void main()
    {
        ns_query::test_storein();
        ns_query::test_quote1();
        ns_query::test_quote2();
        ns_query::test_template();
        std::cout << "hello, mysqlpp\n";
    }
}  // namespace ns_api