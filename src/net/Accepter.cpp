#include "Accepter.h"

#include "IoContextPool.h"
#include "Log.h"

namespace cncpp
{
    TcpAccepter::TcpAccepter(const uint16_t port)
        : listen_port_(port), acceptor_(sIoContextPool.get_io_context() ,
                                  boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
    {
    }

    void TcpAccepter::setAcceptCallback(const AcceptCallBack& cb)
    {
        accept_callback_ = cb;
    }
 
    void TcpAccepter::startAccept()
    {
        INFO("server start listen on port: {}", listen_port_);
        
        IO_CONTEXT& ioc = sIoContextPool.get_io_context();
        SOCKET_PTR  sock = std::make_shared<boost::asio::ip::tcp::socket>(ioc);
        
        auto accept_cb = [this, sock, &ioc](const boost::system::error_code& ec)
        {
            if (accept_callback_)
                accept_callback_(ioc, sock, ec);

            startAccept();
        };

        acceptor_.async_accept(*sock, accept_cb);
    }

    void TcpAccepter::handleAcpErr(SOCKET_PTR sock, const boost::system::error_code& err)
    {
        boost::system::error_code ec;
        sock->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        sock->close(ec);
        stopAccept();
    }

    void TcpAccepter::stopAccept()
    {
        boost::system::error_code err;
        acceptor_.cancel(err);
        acceptor_.close(err);
    }
}