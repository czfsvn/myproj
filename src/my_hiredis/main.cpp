#include <hiredis.h>
#include <winsock2.h>
//#include <Winsock2.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <stdexcept>

#include <adapters/libevent.h>

#include "RedisConn.h"
#include "ConnectionPool.h"

#pragma comment(lib, "ws2_32.lib")

#define REDIS_IP "192.168.98.63"

#ifdef _MSC_VER
#define strdup _strdup
#endif

using namespace std;

class winsock_initializer
{
public:
    winsock_initializer()
    {
        //! Windows network DLL init
        WORD    version = MAKEWORD(2, 2);
        WSADATA data;

        if (WSAStartup(version, &data) != 0)
        {
            throw std::runtime_error("WSAStartup() failure");
        }
    }
    ~winsock_initializer()
    {
        WSACleanup();
    }
};

namespace ns_base_example
{
    int main()
    {
        winsock_initializer init;

        redisContext* c = redisConnect(REDIS_IP, 6379);
        if (c == NULL || c->err)
        {
            if (c)
            {
                printf("Error: %s\n", c->errstr);
                redisFree(c);
            }
            else
            {
                printf("Can't allocate redis context\n");
            }
            return 1;
        }

        // ... 使用 hiredis ...

        redisFree(c);
        return 0;
    }
}

namespace hiredis_example_c
{
    static void example_argv_command(redisContext* c, size_t n)
    {
        char **     argv, tmp[42];
        size_t*     argvlen;
        redisReply* reply;

        /* We're allocating two additional elements for command and key */
        argv    = ( char** )malloc(sizeof(*argv) * (2 + n));
        argvlen = ( size_t* )malloc(sizeof(*argvlen) * (2 + n));

        /* First the command */
        argv[0]    = ( char* )"RPUSH";
        argvlen[0] = sizeof("RPUSH") - 1;

        /* Now our key */
        argv[1]    = ( char* )"argvlist";
        argvlen[1] = sizeof("argvlist") - 1;

        /* Now add the entries we wish to add to the list */
        for (size_t i = 2; i < (n + 2); i++)
        {
            argvlen[i] = snprintf(tmp, sizeof(tmp), "argv-element-%zu", i - 2);
            argv[i]    = strdup(tmp);
        }

        /* Execute the command using redisCommandArgv.  We're sending the arguments with
         * two explicit arrays.  One for each argument's string, and the other for its
         * length. */
        reply = ( redisReply* )redisCommandArgv(
            c, n + 2, ( const char** )argv, ( const size_t* )argvlen);

        if (reply == NULL || c->err)
        {
            fprintf(stderr, "Error:  Couldn't execute redisCommandArgv\n");
            exit(1);
        }

        if (reply->type == REDIS_REPLY_INTEGER)
        {
            printf("%s reply: %lld\n", argv[0], reply->integer);
        }

        freeReplyObject(reply);

        /* Clean up */
        for (size_t i = 2; i < (n + 2); i++)
        {
            free(argv[i]);
        }

        free(argv);
        free(argvlen);
    }


    int main()
    {
        unsigned int  j, isunix = 0;
        redisContext* c;
        redisReply*   reply;
        const char*   hostname = REDIS_IP;

        int port = 6379;

        struct timeval timeout = { 1, 500000 };  // 1.5 seconds
        if (isunix)
        {
            c = redisConnectUnixWithTimeout(hostname, timeout);
        }
        else
        {
            c = redisConnectWithTimeout(hostname, port, timeout);
        }
        if (c == NULL || c->err)
        {
            if (c)
            {
                printf("Connection error: %s\n", c->errstr);
                redisFree(c);
            }
            else
            {
                printf("Connection error: can't allocate redis context\n");
            }
            exit(1);
        }


        /* PING server */
        reply = ( redisReply* )redisCommand(c, "PING");
        printf("PING: %s\n", reply->str);
        freeReplyObject(reply);

        /* Set a key */
        reply = ( redisReply* )redisCommand(c, "SET %s %s", "foo", "hello world");
        printf("SET: %s\n", reply->str);
        freeReplyObject(reply);

        /* Set a key using binary safe API */
        reply =
            ( redisReply* )redisCommand(c, "SET %b %b", "bar", ( size_t )3, "hello", ( size_t )5);
        printf("SET (binary API): %s\n", reply->str);
        freeReplyObject(reply);

        /* Try a GET and two INCR */
        reply = ( redisReply* )redisCommand(c, "GET foo");
        printf("GET foo: %s\n", reply->str);
        freeReplyObject(reply);

        reply = ( redisReply* )redisCommand(c, "INCR counter");
        printf("INCR counter: %lld\n", reply->integer);
        freeReplyObject(reply);
        /* again ... */
        reply = ( redisReply* )redisCommand(c, "INCR counter");
        printf("INCR counter: %lld\n", reply->integer);
        freeReplyObject(reply);

        /* Create a list of numbers, from 0 to 9 */
        reply = ( redisReply* )redisCommand(c, "DEL mylist");
        freeReplyObject(reply);
        for (j = 0; j < 10; j++)
        {
            char buf[64];

            snprintf(buf, 64, "%u", j);
            reply = ( redisReply* )redisCommand(c, "LPUSH mylist element-%s", buf);
            freeReplyObject(reply);
        }

        /* Let's check what we have inside the list */
        reply = ( redisReply* )redisCommand(c, "LRANGE mylist 0 -1");
        if (reply->type == REDIS_REPLY_ARRAY)
        {
            for (j = 0; j < reply->elements; j++)
            {
                printf("%u) %s\n", j, reply->element[j]->str);
            }
        }
        freeReplyObject(reply);

        /* See function for an example of redisCommandArgv */
        example_argv_command(c, 10);

        /* Disconnects and frees the context */
        redisFree(c);

        return 0;
    }
}

namespace ns_hiredis_libevent
{
    void getCallback(redisAsyncContext* c, void* r, void* privdata)
    {
        redisReply* reply = ( redisReply* )r;
        if (reply == NULL)
        {
            if (c->errstr)
            {
                printf("errstr: %s\n", c->errstr);
            }
            return;
        }
        printf("argv[%s]: %s\n", ( char* )privdata, reply->str);

        /* Disconnect after receiving the reply to GET */
        //redisAsyncDisconnect(c);
    }

    void connectCallback(const redisAsyncContext* c, int status)
    {
        if (status != REDIS_OK)
        {
            printf("Error: %s\n", c->errstr);
            return;
        }
        printf("Connected...\n");
    }

    void disconnectCallback(const redisAsyncContext* c, int status)
    {
        if (status != REDIS_OK)
        {
            printf("Error: %s\n", c->errstr);
            return;
        }
        printf("Disconnected...\n");
    }

    int main(int argc, char** argv)
    {
#ifndef _WIN32
        signal(SIGPIPE, SIG_IGN);
#endif
        winsock_initializer init;
        struct event_base* base    = event_base_new();
        redisOptions       options = { 0 };
        REDIS_OPTIONS_SET_TCP(&options, REDIS_IP, 6379);
        struct timeval tv       = { 0 };
        tv.tv_sec               = 1;
        options.connect_timeout = &tv;

        redisAsyncContext* c = redisAsyncConnectWithOptions(&options);
        if (c->err)
        {
            /* Let *c leak for now... */
            printf("Error: %s\n", c->errstr);
            return 1;
        }

        redisLibeventAttach(c, base);
        redisAsyncSetConnectCallback(c, connectCallback);
        redisAsyncSetDisconnectCallback(c, disconnectCallback);
        redisAsyncCommand(c, NULL, NULL, "SET key %b", argv[argc - 1], strlen(argv[argc - 1]));
        redisAsyncCommand(c, getCallback, ( char* )"end-1", "GET key");
        event_base_dispatch(base);
        return 0;
    }
}

namespace ns_syncredis
{
    void test1()
    {
        cncpp::RedisConn redis(REDIS_IP, 6379);
        // 设置值

        auto set_result = redis.command("SET username alice");
        std::cout << "SET result: " << set_result.toString() << "\n";

        // 获取值
        auto get_result = redis.command("GET username");
        if (!get_result.isNull())
        {
            std::cout << "Username: " << get_result.toString() << "\n";
        }

        // 处理列表
        redis.command("RPUSH cart item1 item2 item3");
        auto list_result = redis.command("LRANGE cart 0 -1");

        if (list_result.isArray())
        {
            std::cout << "Cart items:\n";
            for (const auto& item : list_result.toArray())
            {
                std::cout << "- " << item.toString() << "\n";
            }
        }

        // 处理错误
        try
        {
            auto error_result = redis.command("INVALIDCOMMAND");
        }
        catch (const std::exception& e)
        {
            std::cerr << "Command failed: " << e.what() << "\n";
        }
    }

    void test2()
    {
        cncpp::RedisConn redis(REDIS_IP, 6379);
        // 设置值

        redis.command("SET username alice").dumpResult();

        // 获取值
        redis.command("GET username").dumpResult();

        // 处理列表
        redis.command("RPUSH cart item1 item2 item3").dumpResult();
        redis.command("LRANGE cart 0 -1").dumpResult();

        // 处理错误
        try
        {
            redis.command("INVALIDCOMMAND").dumpResult();
        }
        catch (const std::exception& e)
        {
            std::cerr << "Command failed: " << e.what() << "\n";
        }
    }


    void test3()
    {
        {
            ScopedRedisConn redis;
            // 设置值

            redis->command("SET username alice").dumpResult();

            // 获取值
            redis->command("GET username").dumpResult();
        }
    }

    void main() 
    {
        //test1();
        //test2();
        test3();
    }

}


int main(int argc, char** argv)
{

    cncpp::RedisConfig config;
    config.host    = REDIS_IP;
    config.dbindex = 1;
    config.port    = 6379;

    RedisConnectPool::getMe().init(config);

    //ns_base_example::main();
    //hiredis_example_c::main();

    //ns_hiredis_libevent::main(argc, argv);
    ns_syncredis::main();
    return 0;
}