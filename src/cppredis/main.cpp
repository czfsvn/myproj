#include <cpp_redis/cpp_redis>
#include <iostream>
#include <tacopie/tacopie>
//#include <winsock2.h>

#include <string>
#include <cpp_redis/misc/macro.hpp>


#ifdef _WIN32
#include <Winsock2.h>
#include <stdexcept>

#define REDIS_IP    "192.168.98.63"

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
#else
class winsock_initializer
{
};
#endif /* _WIN32 */

namespace cpp_redis_example
{
    int cpp_redis_client()
    {

        winsock_initializer winsock_init;
        //! Enable logging
        cpp_redis::active_logger = std::unique_ptr<cpp_redis::logger>(new cpp_redis::logger);

        cpp_redis::client client;

        client.connect(REDIS_IP, 6379,
            [](const std::string& host, std::size_t port, cpp_redis::client::connect_state status)
            {
                if (status == cpp_redis::client::connect_state::dropped)
                {
                    std::cout << "client disconnected from " << host << ":" << port << std::endl;
                }
            });


#if 1
        auto replcmd = [](const cpp_redis::reply& reply)
        {
            std::cout << "set hello 42: " << reply << std::endl;
            // if (reply.is_string())
            //   do_something_with_string(reply.as_string())
        };

        const std::string group_name    = "groupone";
        const std::string session_name  = "sessone";
        const std::string consumer_name = "ABCD";

        std::multimap<std::string, std::string> ins;
        ins.insert(std::pair<std::string, std::string>{ "message", "hello" });

#ifdef ENABLE_SESSION

        client.xadd(session_name, "*", ins, replcmd);
        client.xgroup_create(session_name, group_name, "0", replcmd);

        client.sync_commit();

        client.xrange(session_name, { "-", "+", 10 }, replcmd);

        client.xreadgroup(
            {
                group_name, consumer_name, { { session_name }, { ">" } },
                1,      // Count
                0,      // block milli
                false,  // no ack
            },
            [](cpp_redis::reply& reply)
            {
                std::cout << "set hello 42: " << reply << std::endl;
                auto msg = reply.as_array();
                std::cout << "Mes: " << msg[0] << std::endl;
                // if (reply.is_string())
                //   do_something_with_string(reply.as_string())
            });

#else

        // same as client.send({ "SET", "hello", "42" }, ...)
        client.set("hello", "42",
            [](cpp_redis::reply& reply)
            {
                std::cout << "set hello 42: " << reply << std::endl;
                // if (reply.is_string())
                //   do_something_with_string(reply.as_string())
            });

        // same as client.send({ "DECRBY", "hello", 12 }, ...)
        client.decrby("hello", 12,
            [](cpp_redis::reply& reply)
            {
                std::cout << "decrby hello 12: " << reply << std::endl;
                // if (reply.is_integer())
                //   do_something_with_integer(reply.as_integer())
            });

        // same as client.send({ "GET", "hello" }, ...)
        client.get("hello",
            [](cpp_redis::reply& reply)
            {
                std::cout << "get hello: " << reply << std::endl;
                // if (reply.is_string())
                //   do_something_with_string(reply.as_string())
            });

#endif

        // commands are pipelined and only sent when client.commit() is called
        // client.commit();

        // synchronous commit, no timeout
        client.sync_commit();

        // synchronous commit, timeout
        // client.sync_commit(std::chrono::milliseconds(100));
        #endif
        return 0;
    }
}

int test()
{
    // 初始化网络库

    winsock_initializer winsock_init;

    // 创建客户端
    cpp_redis::client client;

    try
    {
        client.connect("192.168.98.63", 6379);
        client.set("test", "success");
        auto reply = client.get("test");
        client.sync_commit();

        if (reply.get().as_string() == "success")
        {
            std::cout << "cpp-redis 安装成功!" << std::endl;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "错误: " << e.what() << std::endl;
        return 1;
    }
}

int main()
{
    cpp_redis_example::cpp_redis_client();
    return 0;    
}