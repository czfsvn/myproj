#include <boost/asio.hpp>
#include <bredis/Connection.hpp>
#include <iostream>

namespace boost_sync
{
    int main()
    {
        boost::asio::io_context      io_ctx;
        boost::asio::ip::tcp::socket socket(io_ctx);

        // 连接 Redis
        socket.connect(
            boost::asio::ip::tcp::endpoint(boost::asio::ip::make_address("127.0.0.1"), 6379));

        bredis::Connection<decltype(socket)> conn(std::move(socket));

        // 发送 SET 命令
        conn.write("SET foo bar");

        // 读取响应
        auto parse_result = conn.read();
        std::cout << "SET 响应: " << parse_result << std::endl;

        // 发送 GET 命令
        conn.write("GET foo");
        parse_result = conn.read();
        std::cout << "GET 响应: " << parse_result << std::endl;

        return 0;
    }
}

int main()
{
    boost_sync::main();
    reutrn 1;
}