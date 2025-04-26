#pragma once
#include "NetDefine.h"
#include "TcpSession.h"

namespace cncpp
{
    class AsyncClient : public std::enable_shared_from_this<AsyncClient>
    {
    public:
        AsyncClient(IO_CONTEXT& ioc, const std::string& host, const uint16_t port);

        void connect();

        void onConnected();

    private:
        boost::asio::ip::tcp::resolver resolver_;
        cncpp::TcpSession              session_;
        std::string                    server_host_ = {};
        uint16_t                       server_port_ = {};
    };
}  // namespace cncpp