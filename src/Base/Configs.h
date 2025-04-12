#pragma once

#include <string>

namespace cncpp 
{
	// 连接配置
    struct MysqlConfig
    {
        std::string host     = {};
        std::string user     = {};
        std::string password = {};
        std::string database = {};
        uint32_t    port     = 3306;
        const char* charset  = "utf8mb4";
    };

    // redis connection config
    struct RedisConfig
    {
        std::string host    = {};
        uint32_t    port    = 3306;
        uint32_t    dbindex = 0;
    };
} // namespace cncpp