#include "RedisConn.h"
#include <iostream>



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
}