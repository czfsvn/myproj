#include "Stock.h"

Stock::Stock(const mysqlpp::Row& row)
{
	item_ = row["item"];
	num_ = row["num"];
	weight_ = row["weight"];
	price_ = row["price"];
	sdate_ = row["sdate"];
	description_ = row["description"];
}