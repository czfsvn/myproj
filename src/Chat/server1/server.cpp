#include "server.h"
#include "AsioTimer.h"
#include "IoContextPool.h"

namespace server1
{
    void Server::init(const uint16_t io_pool_size, const uint16_t port)
    {
        io_pool_size_ = io_pool_size;
        listen_port_  = port;

        sIoContextPool.init(io_pool_size_);
        main_io_context_ = &sIoContextPool.get_io_context();

        {
            sAsioTimer.init(main_io_context_, 10000);
            sAsioTimer.setCallBack(std::bind(&Server::timerCallBack, this));
        }

        acceptor_ = std::make_shared<cncpp::TcpAccepter>(*main_io_context_, port);
    }

    void Server::run()
    {
        acceptor_->startAccept();
        sAsioTimer.start();
        sIoContextPool.run();
    }

    void Server::timerCallBack()
    {
        INFO("[Server][timerCallBack]");
    }
}  // namespace server1