#pragma once

#include "Accepter.h"
#include "Singleton.h"
#include "NetDefine.h"


namespace server1
{
    class Server : public cncpp::Singleton<Server>
    {
    public:
        Server(){}

        void init(const uint16_t io_pool_size, const uint16_t port);

        void run();

        void timerCallBack();

    private:
        std::shared_ptr<cncpp::TcpAccepter> acceptor_        = {};
        cncpp::IO_CONTEXT*                  main_io_context_ = {};
        
        uint16_t listen_port_ = 0;
        uint16_t io_pool_size_ = 0;
    };
}  // namespace server1

#define sServer server1::Server::getMe()