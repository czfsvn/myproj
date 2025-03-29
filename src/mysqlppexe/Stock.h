#pragma once

#include <mysql++.h>
#include "Log.h"

class Stock
{
public:
	Stock(const mysqlpp::Row& row);

	void print()
	{
		INFO("{}\t, {}\t, {}\t, {}\t, {}\t, {}\t", item_, num_, weight_, price_, sdate_, description_);
	}

private:
	std::string item_ = {};
	uint64_t num_ = 0;
	double weight_ = 0.0;
	double price_ = 0.0;
	std::string sdate_ = "";
	std::string description_ = "";
};