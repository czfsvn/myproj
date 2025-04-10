#include "Stock.h"
//#include <sstream>
//#include "Pool.h"


Stock::Stock(const mysqlpp::Row& row)
{
	item_ = row["item"];
	num_ = row["num"];
	weight_ = row["weight"];
	price_ = row["price"];
	sdate_ = row["sdate"];
	description_ = row["description"];
}

Stock::Stock(const std::string& item, const std::string& num, const std::string& weight,
    const std::string& price, const std::string& sdate, const std::string& description)
{
    item_        = item;
    num_         = std::stoll(num);
    weight_      = std::stod(weight);
    price_       = std::stod(price);
    sdate_       = sdate;
    description_ = description;
}

std::string Stock::field_list()
{
	std::string ret = "item, num, weight, price, sdate, description";
	return ret;
}


std::string Stock::value_list() const
{
    const std::string singlequote = "'";
    const std::string conn_str    = singlequote + "," + singlequote;

    // tobe optimized to std::ostringstream
    std::string ret = singlequote + item_ + conn_str + std::to_string(num_) + conn_str
                      + std::to_string(weight_) + conn_str + std::to_string(price_) + conn_str
                      + sdate_ + conn_str + description_ + singlequote;
    return ret;
}

std::string Stock::table()
{
    return "stock";
}

std::vector<Stock> Stock::loadWhere(const std::string& where)
{
    ScopedMySqlConn con;
    return con->loadWhere<Stock>(where);
}

bool Stock::deleteWhere(const std::string& where)
{
    ScopedMySqlConn con;
    return con->deleteWhere<Stock>(where);
}


uint32_t Stock::replaceAll(const std::vector<Stock>& cont) 
{
    ScopedMySqlConn con;
    return con->replaceAll<Stock>(cont);
}

uint32_t Stock::replaceDB() const
{
    ScopedMySqlConn con;
    return con->replaceDB<Stock>(*this);
}
