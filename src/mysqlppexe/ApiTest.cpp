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
        st.dump();
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
            item.dump();
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

    void main()
    {
        test_storein();
        test_quote1();
        test_quote2();
        test_template();
    }
}  // namespace ns_query

namespace ns_stockapi
{
    void test_loadAll()
    {
        vector<Stock> res = Stock::loadWhere("");
        for (const auto& item : res)
        {
            item.dump();
        }

        vector<Stock> res2 = Stock::loadWhere("num=100");
        for (const auto& item : res2)
        {
            item.dump();
        }
    }

    void test_delete()
    {
        Stock::deleteWhere("num=100");            
    }

    void main()
    {
        test_loadAll();
        test_delete();
    }
}

namespace ns_insert
{
    std::vector<Stock> stock_vec;

    size_t tokenize_line(const string& line, vector<std::string>& strings)
    {
        string field;
        strings.clear();

        istringstream iss(line);
        while (getline(iss, field, '\t'))
        {
            strings.push_back(std::string(field));
        }

        return strings.size();
    }

    bool read_stock_items(const char* filename, vector<Stock>& stock_vector)
    {
        ifstream input(filename);
        if (!input)
        {
            cerr << "Error opening input file '" << filename << "'" << endl;
            return false;
        }

        string                  line;
        vector<std::string> strings;
        while (getline(input, line))
        {
            if (tokenize_line(line, strings) == 6)
            {
                stock_vector.push_back(Stock(string(strings[0]), strings[1], strings[2], strings[3],
                    strings[4], strings[5]));
            }
            else
            {
                cerr << "Error parsing input line (doesn't have 6 fields) "
                     << "in file '" << filename << "'" << endl;
                cerr << "invalid line: '" << line << "'" << endl;
            }
        }

        return true;
    }

    void test_replaceall()
    {
        if (!read_stock_items(
                "D:\\WorkCodes\\open_src\\mysql++-3.3.0\\examples\\stock.txt", stock_vec))
            return;

        Stock::deleteWhere("");

        Stock::replaceAll(stock_vec);

        for (const auto& item : stock_vec)
        {
            item.replaceDB();
        }

        //ScopedMySqlConn                         con;
        //mysqlpp::Query::MaxPacketInsertPolicy<> insert_policy(1000);
        //con->getConn().query().replacefrom(stock_vec.begin(), stock_vec.end(), insert_policy);
    }
    void main()
    {
        test_replaceall();
    }
}

namespace ns_mysqlconapi
{
    void load()
    {
        {
            const std::vector<Stock>& stock_vec = Stock::loadWhere("");
            for (const Stock& stock : stock_vec)
            {
                stock.dump();
            }
        }
        
        {
            const std::vector<Stock>& stock_vec = Stock::loadWhere("num=60");
            for (const Stock& stock : stock_vec)
            {
                stock.dump();
            }
        }

        {
            ScopedMySqlConn con;
            const std::vector<Stock>& stock_vec = con->loadWhere<Stock>("num=60");
            for (const Stock& stock : stock_vec)
            {
                stock.dump();
            }
        }         

        return;
    }
    void main()
    {
        load();
    }
}

namespace ns_api
{
    void main()
    {
        //ns_query::main();

        //ns_stockapi::main();

        ns_insert::main();

        ns_mysqlconapi::main();
        std::cout << "hello, mysqlpp\n";
    }
}  // namespace ns_api