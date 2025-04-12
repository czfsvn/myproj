#pragma once

#include <hiredis.h>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "Configs.h"
#include "ConnectionPool.h"

namespace cncpp
{
    class RedisReply
    {
    public:
        explicit RedisReply(redisReply* reply);
        RedisReply() {}

        void reset();

        // 类型检查方法
        bool isNull() const;
        bool isString() const;
        bool isArray() const;
        bool isInteger() const;
        bool isStatus() const;
        bool isError() const;

        void dumpResult() const;

        int64_t                 toInt() const;
        const std::string       toError() const;
        std::string             toString() const;
        std::vector<RedisReply> toArray() const;

    private:
        bool parseFrom(redisReply* reply);
        

    private:
        int16_t                 type_    = 0;
        int64_t                 integer_ = 0;
        std::string             str_     = {};
        std::vector<RedisReply> row_     = {};
    };

    class RedisConn
    {
    public:

        RedisConn(const RedisConfig& cfg);
        RedisConn(const std::string& host, int port);
        ~RedisConn();

        // 禁止拷贝
        RedisConn(const RedisConn&)            = delete;
        RedisConn& operator=(const RedisConn&) = delete;

        // 允许移动
        RedisConn(RedisConn&& other) noexcept;
        RedisConn& operator=(RedisConn&& other) noexcept;

        bool isConnected();

        RedisReply command(const std::string& cmd);

        RedisReply command_v(const char* format, va_list ap);

        RedisReply command_argv(const std::vector<std::string>& args);


        bool select(int32_t index);

    private:       

        void checkConnection();

        void connect();

    private:
        redisContext* context_ = nullptr;
        RedisConfig config_ = {};
    };
}  // namespace cncpp

using RedisConnectPool = cncpp::MyConnPool<cncpp::RedisConn, cncpp::RedisConfig>;
using ScopedRedisConn  = cncpp::MyScopedConn<cncpp::RedisConn, cncpp::RedisConfig>;