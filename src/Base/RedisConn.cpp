#include "RedisConn.h"
#include <iostream>
#include "Log.h"


namespace cncpp
{
    void printRedisValue(const RedisReply& value, int indent = 0)
    {
        std::string prefix(indent, ' ');

        if (value.isNull())
        {
            std::cout << prefix << "null\n";
        }
        else if (value.isError())
        {
            std::cout << prefix << "Error: " << value.toError() << "\n";
        }
        else if (value.isString())
        {
            std::cout << prefix << "String: \"" << value.toString() << "\"\n";
        }
        else if (value.isInteger())
        {
            std::cout << prefix << "Integer: " << value.toInt() << "\n";
        }
        else if (value.isArray())
        {
            std::cout << prefix << "Array [\n";
            for (const auto& item : value.toArray())
            {
                printRedisValue(item, indent + 2);
            }
            std::cout << prefix << "]\n";
        }
        else if (value.isStatus())
        {
            std::cout << prefix << "Status: \"" << value.toString() << "\"\n";
        }
    }

    RedisReply::RedisReply(redisReply* reply)
    {
        parseFrom(reply);
    }

    void RedisReply::reset()
    {
        type_    = 0;
        integer_ = 0;
        str_     = "";
        row_.clear();
    }

    bool RedisReply::parseFrom(redisReply* reply)
    {
        if (!reply)
            return false;

        reset();

        type_ = reply->type;
        if (REDIS_REPLY_INTEGER == type_)
        {
            integer_ = reply->integer;
            return true;
        }
        else if (REDIS_REPLY_ERROR == type_ || REDIS_REPLY_STRING == type_
                 || REDIS_REPLY_STATUS == type_)
        {
            str_.assign(reply->str, reply->len);
            return true;
        }
        if (REDIS_REPLY_ARRAY == type_)
        {
            for (size_t i = 0; i < reply->elements; i++)
            {
                //std::string element;
                //element.assign(reply->element[i]->str, reply->element[i]->len);
                //elements.push_back(element);

                row_.emplace_back(reply->element[i]);
            }
            return true;
        }            
        return false;
    }

    int64_t RedisReply::toInt() const
    {
        if (!isInteger())
            return (-1);

        return integer_;
    }

    std::string RedisReply::toString() const
    {
        if (!isString() && !isError() && !isStatus())
            return "";

        return str_;
    }

    const std::string RedisReply::toError() const
    {
        if (!isError())
            return "";

        return str_;
    }

    std::vector<RedisReply> RedisReply::toArray() const
    {
        if (!isArray())
            return {};

        return row_;
    }

    void RedisReply::dumpResult() const
    {
        printRedisValue(*this);
    }

            // 类型检查方法
    bool RedisReply::isNull() const
    {
        return type_ == REDIS_REPLY_NIL;
    }

    bool RedisReply::isString() const
    {
        return type_ == REDIS_REPLY_STRING;
    }

    bool RedisReply::isArray() const
    {
        return type_ == REDIS_REPLY_ARRAY;
    }

    bool RedisReply::isInteger() const
    {
        return type_ == REDIS_REPLY_INTEGER;
    }

    bool RedisReply::isStatus() const
    {
        return type_ == REDIS_REPLY_STATUS;
    }

    bool RedisReply::isError() const
    {
        return type_ == REDIS_REPLY_ERROR;
    }


    RedisConn::RedisConn(const RedisConfig& cfg) : config_(cfg)
    { 
        connect();
    }

    RedisConn::RedisConn(const std::string& host, int port)
    {
        context_ = redisConnect(host.c_str(), port);
        if (!context_ || context_->err)
        {
            throw std::runtime_error(context_ ? context_->errstr : "Can't allocate context");
        }
    }

    RedisConn::~RedisConn()
    {
        if (context_)
        {
            redisFree(context_);
            context_ = nullptr;
        }
    }

    void RedisConn::connect()
    {
        context_ = redisConnect(config_.host.c_str(), config_.port);
        if (!context_ || context_->err)
        {
            throw std::runtime_error(context_ ? context_->errstr : "Can't allocate context");
        }

        if (config_.dbindex)
            select(config_.dbindex);

        TRACE("[RedisConn] connect success");
    }

    bool RedisConn::isConnected()
    {
        return context_ && context_->err == 0;
    }

    void RedisConn::checkConnection()
    {
        if (!isConnected())
            connect();
    }

    RedisReply RedisConn::command(const std::string& cmd)
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

    RedisReply RedisConn::command_v(const char* format, va_list ap)
    {
        checkConnection();
        if (!isConnected())
            return {};

        redisReply* reply = ( redisReply* )redisvCommand(context_, format, ap);
        if (!reply)
        {
            throw std::runtime_error("Command failed");
        }

        // 使用智能指针自动管理内存
        std::unique_ptr<redisReply, void (*)(void*)> reply_guard(reply, freeReplyObject);

        return RedisReply(reply);
    }

    RedisReply RedisConn::command_argv(const std::vector<std::string>& args)
    {
        checkConnection();
        if (!isConnected())
            return {};

        int     argc    = ( int )args.size();
        char**  argv    = new char*[args.size()];
        size_t* argvlen = new size_t[args.size()];
        for (size_t i = 0; i < args.size(); i++)
        {
            argv[i]    = ( char* )args[i].data();
            argvlen[i] = args[i].size();
        }

        redisReply* reply =
            ( redisReply* )redisCommandArgv(context_, argc, ( const char** )argv, argvlen);
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

    // 允许移动
    RedisConn::RedisConn(RedisConn&& other) noexcept : context_(other.context_)
    {
        other.context_ = nullptr;
    }

    RedisConn& RedisConn::operator=(RedisConn&& other) noexcept
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


    bool RedisConn::select(int32_t index)
    {
        RedisReply reply  = command(fmt::format("SELECT {}", index));
        return reply.isStatus() && reply.toString() == "OK";
    }
}