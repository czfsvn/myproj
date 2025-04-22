#include "TcpClient.h"

namespace cncpp
{
    using boost::asio::ip::tcp;

    AsyncClient::AsyncClient(IO_CONTEXT& ioc, const std::string& host, const uint16_t port)
        : server_host_(host), server_port_(port), resolver_(ioc),
          session_(ioc, std::make_shared<boost::asio::ip::tcp::socket>(ioc))
    {
        // todo: session set callback
    }

    void AsyncClient::connect()
    {
        auto connect_cb = [this](boost::system::error_code ec, tcp::endpoint)
        {
            if (!ec)
            {
                onConnected();
            }
        };

        auto endpoints = resolver_.resolve(server_host_, std::to_string(server_port_));
        boost::asio::async_connect(*(session_.getSocket()), endpoints, connect_cb);
    }

    void AsyncClient::onConnected()
    {
        INFO("[AsyncClient][onConnected] connected\n");
        session_.start();
    }
}  // namespace cncpp