#pragma once

#include <string>

namespace cncpp
{
    // ¡¨Ω”≈‰÷√
    struct MysqlConfig
    {
        std::string host = {};
        std::string user = {};
        std::string password = {};
        std::string database = {};
        unsigned int port = 3306;
        const char* charset = "utf8mb4";
    };
}