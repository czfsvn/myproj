#pragma once

#include <mysql++.h>
#include "Log.h"

class Stock
{
public:
	Stock(const mysqlpp::Row& row);


	static std::vector<Stock> loadAll(const std::string& where);
    static bool               deleteWhere(const std::string& where);
    static bool               replaceAll(const std::vector<Stock>& cont);

	void replaceDB();
	void updateDB();
	void insertDB();
	void selectDB(const char* where = NULL);

	void print() const
	{
		INFO("{}\t, {}\t, {}\t, {}\t, {}\t, {}\t", item_, num_, weight_, price_, sdate_, description_);
	}

	static std::string FieldList();

	std::string ValueList();

private:
	std::string item_ = {};
	uint64_t num_ = 0;
	double weight_ = 0.0;
	double price_ = 0.0;
	std::string sdate_ = "";
	std::string description_ = "";
};