#include "Stock.h"
//#include <sstream>


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

#if 0
std::string Stock::ValueList()
{
	std::ostringstream oss;
	oss << "`" << item_ << "`,`" << num_ << "`,`" << weight_ << "`,`" 
		<< price_ << "`,`" << sdate_ << "`,`" << description_ << "`";
	return oss.str();
}


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