#include <cpp_redis/cpp_redis>
#include <iostream>
#include <tacopie/tacopie>

int main()
{
    // 初始化网络库
    //tacopie::network::initialize();

    // 创建客户端
    cpp_redis::client client;

    // 连接Redis服务器
    client.connect("192.168.98.63", 6379,
        [](const std::string& host, std::size_t port, cpp_redis::connect_state status)
        {
            if (status == cpp_redis::connect_state::dropped)
            {
                std::cerr << "Client disconnected from " << host << ":" << port << std::endl;
            }
        });

    // 设置键值
    client.set("hello", "world");

    // 获取键值
    client.get("hello",
        [](cpp_redis::reply& reply)
        {
            if (reply.is_string())
            {
                std::cout << "Got value: " << reply.as_string() << std::endl;
            }
        });

    // 提交命令
    client.commit();

    // 同步等待
    std::this_thread::sleep_for(std::chrono::seconds(1));

    return 0;
}