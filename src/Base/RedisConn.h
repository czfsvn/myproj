#pragma once

#include <string>
#include <vector>
#include <hiredis.h>
#include <stdexcept>
#include <memory>

namespace cncpp
{
    class RedisReply
    {
    public:
        explicit RedisReply(redisReply* reply);

            // 禁止拷贝
        //RedisReply(const RedisReply&)            = delete;
        //RedisReply& operator=(const RedisReply&) = delete;
        #if 0
        // 支持移动语义
        RedisReply(RedisReply&& other) noexcept
            : type_(other.type_), integer_(other.integer_), str_(std::move(other.str_)),
              row_(std::move(other.row_))
        {
            other.reset();
        }

        RedisReply& operator=(RedisReply&& other) noexcept
        {
            if (this != &other)
            {
                type_    = other.type_;
                integer_ = other.integer_;
                str_     = std::move(other.str_);
                row_     = std::move(other.row_);
                other.reset();
            }
            return *this;
        }
        #endif
        // 类型检查方法
        bool isNull() const
        {
            return type_ == REDIS_REPLY_NIL;
        }
        bool isString() const
        {
            return type_ == REDIS_REPLY_STRING;
        }
        bool isArray() const
        {
            return type_ == REDIS_REPLY_ARRAY;
        }
        bool isInteger() const
        {
            return type_ == REDIS_REPLY_INTEGER;
        }
        bool isStatus() const
        {
            return type_ == REDIS_REPLY_STATUS;
        }
        bool isError() const
        {
            return type_ == REDIS_REPLY_ERROR;
        }
        
        void                    dumpResult() const;

        int64_t toInt() const;
        const std::string toError() const;
        std::string toString() const;
        std::vector<RedisReply> toArray() const;


    private:
        bool parseFrom(redisReply* reply);
        void reset();
        

    private:
        int16_t                 type_    = 0;
        int64_t                 integer_ = 0;
        std::string             str_     = {};
        std::vector<RedisReply> row_ = {};
    };

    class RedisConn
    {
    public:
        RedisConn(const std::string& host, int port)
        {
            context_ = redisConnect(host.c_str(), port);
            if (!context_ || context_->err)
            {
                throw std::runtime_error(context_ ? context_->errstr : "Can't allocate context");
            }
        }

        ~RedisConn()
        {
            if (context_)
            {
                redisFree(context_);
            }
        }

        bool isConnected()
        {
            return context_ && context_->err == 0;
        } 

        void checkConnection()
        {
            //if (!isConnected())
            //    connect();
        }
        
        RedisReply command(const std::string& cmd)
        {
            redisReply* reply = ( redisReply* )redisCommand(context_, cmd.c_str());
            if (!reply)
            {
                throw std::runtime_error("Command failed");
            }

            // 使用智能指针自动管理内存
            std::unique_ptr<redisReply, void (*)(void*)> reply_guard(reply, freeReplyObject);

            return RedisReply(reply);
        } 

        RedisReply command_v(const char* format, va_list ap)
        {
            //checkConnection();
            //if (!isConnected())
            //    return false;

            redisReply* reply = ( redisReply* )redisvCommand(context_, format, ap);
            if (!reply)
            {
                throw std::runtime_error("Command failed");
            }

            // 使用智能指针自动管理内存
            std::unique_ptr<redisReply, void (*)(void*)> reply_guard(reply, freeReplyObject);

            return RedisReply(reply);
        }

        RedisReply command_argv(const std::vector<std::string>& args)
        {
            //checkConnection();
            //if (!isConnected())
            //    return false;

            int     argc    = ( int )args.size();
            char**  argv    = new char*[args.size()];
            size_t* argvlen = new size_t[args.size()];
            for (size_t i = 0; i < args.size(); i++)
            {
                argv[i]    = ( char* )args[i].data();
                argvlen[i] = args[i].size();
            }

            redisReply* reply = ( redisReply* )redisCommandArgv(context_, argc, ( const char** )argv, argvlen);
            if (!reply)
            {
                throw std::runtime_error("Command failed");
            }

            delete[] argv;
            delete[] argvlen;

            // 使用智能指针自动管理内存
            std::unique_ptr<redisReply, void (*)(void*)> reply_guard(reply, freeReplyObject);

            return RedisReply(reply);
        }
        

        // 禁止拷贝
        RedisConn(const RedisConn&)              = delete;
        RedisConn& operator=(const RedisConn&) = delete;

        // 允许移动
        RedisConn(RedisConn&& other) noexcept : context_(other.context_)
        {
            other.context_ = nullptr;
        }

        RedisConn& operator=(RedisConn&& other) noexcept
        {
            if (this != &other)
            {
                if (context_)
                    redisFree(context_);
                context_       = other.context_;
                other.context_ = nullptr;
            }
            return *this;
        }

    private:
        redisContext* context_;
    };
}  // namespace cncpp