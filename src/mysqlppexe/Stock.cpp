#include "Stock.h"
//#include <sstream>
#include "Pool.h"


Stock::Stock(const mysqlpp::Row& row)
{
	item_ = row["item"];
	num_ = row["num"];
	weight_ = row["weight"];
	price_ = row["price"];
	sdate_ = row["sdate"];
	description_ = row["description"];
}

std::string Stock::FieldList()
{
	//return "";
	//std::ostringstream oss;
	//oss << "item, num, weight, price, sdate, description";
	//return  oss.str();
	std::string ret = "item, num, weight, price, sdate, description";
	return ret;
}


std::string Stock::ValueList()
{
	//std::ostringstream oss;
	//oss << "`" << item_ << "`,`" << num_ << "`,`" << weight_ << "`,`" 
	//	<< price_ << "`,`" << sdate_ << "`,`" << description_ << "`";
	//return oss.str();
	
	//mysqlpp::SQLQueryParms oss;
    //oss << "`" << item_ << "`,`" << num_ << "`,`" << weight_ << "`,`"
    //	<< price_ << "`,`" << sdate_ << "`,`" << description_ << "`";
    //return std::string(oss.data(), oss.size());

    return "";
}

std::vector<Stock> Stock::loadAll(const std::string& where) 
{
    ScopedMySqlConn con;
    mysqlpp::Query query = con->getConn().query();
    query << "select " << FieldList() << " from Stock";
    if (where.size())
        query << " where " << where;

	std::vector<Stock> res;
    query.storein(res);  // 需要满足 Stock 构造函数与 row 的转换

	return res;
}

bool Stock::deleteWhere(const std::string& where)
{
    ScopedMySqlConn con;
    mysqlpp::Query  query = con->getConn().query();
    query << "delete  from Stock";
    if (where.size())
        query << " where " << where;

	mysqlpp::SimpleResult res = query.execute();
    if (!res)
		return false;

	return res.rows() > 0;
}

bool Stock::replaceAll(const std::vector<Stock>& cont) 
{
    /*
	mysqlpp::Query::MaxPacketInsertPolicy<> insert_policy(1000);
    replacefrom
    query.replacefrom(stock_vector.begin(), stock_vector.end(),
        insert_policy);

	query.insertfrom(stock_vector.begin(), stock_vector.end(),
		insert_policy);
	*/
	return true;
}

#if 0
void Stock::replaceDB()
{

}

void Stock::updateDB()
{

}

void Stock::insertDB()
{

}

void Stock::selectDB(const char* where/* = NULL*/)
{

}
#endif