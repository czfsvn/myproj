#pragma once

#include <functional>
#include "NetDefine.h"

namespace cncpp
{
	class TcpAccepter
	{
    public:
        TcpAccepter(const uint16_t port);

		void startAccept();

		void setAcceptCallback(const AcceptCallBack& cb);

		void handleAcpErr(SOCKET_PTR sock, const boost::system::error_code& err);

        void stopAccept();

	private:

		boost::asio::ip::tcp::acceptor acceptor_;

        AcceptCallBack accept_callback_ = {};

        uint16_t listen_port_ = 0;
	};
}