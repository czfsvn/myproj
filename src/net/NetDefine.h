#pragma

#include <boost/asio.hpp>
#include "Buffer.h"

namespace cncpp
{
    using IO_CONTEXT     = boost::asio::io_context;
    using SOCKET_PTR     = std::shared_ptr<boost::asio::ip::tcp::socket>;
    using ERR_CODE       = boost::system::error_code;
   

    using AcceptCallBack = std::function<void(IO_CONTEXT&, SOCKET_PTR, const ERR_CODE&)>;

    using MessageCallBack = std::function<void(const Message&)>;

    using TimerCallBack =  std::function<void()>;
}