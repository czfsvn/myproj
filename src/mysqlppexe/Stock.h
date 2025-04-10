#pragma once

#include <mysql++.h>
#include "Log.h"
#include "Pool.h"

class Stock
{
public:
	Stock(const mysqlpp::Row& row);

	Stock(const std::string& item, const std::string& num, const std::string& weight,
        const std::string& price, const std::string& sdate, const std::string& description);


	static std::vector<Stock> loadWhere(const std::string& where);
    static bool               deleteWhere(const std::string& where);
    static uint32_t           replaceAll(const std::vector<Stock>& cont);
    

	uint32_t replaceDB() const;

	void dump() const
	{
		INFO("{}\t, {}\t, {}\t, {}\t, {}\t, {}\t", item_, num_, weight_, price_, sdate_, description_);
	}

	static std::string table();
    static std::string field_list();
    std::string        value_list() const;

private:
    std::string item_        = {};
    uint64_t    num_         = 0;
    double      weight_      = 0.0;
    double      price_       = 0.0;
    std::string sdate_       = "";
    std::string description_ = "";
};
