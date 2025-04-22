#include "TcpSession.h"

#define _MULTI_THREADS

namespace cncpp
{
    TcpSession::TcpSession(IO_CONTEXT& ioc, const SOCKET_PTR sockptr)
        : strand_(ioc), socket_(sockptr), state_(SessionState::eState_verify)
    {
    }

    void TcpSession::setMsgCallback(const MessageCallBack& cb)
    {
        msg_callback_ = cb;
    }

    std::string TcpSession::getRemoteAddr() const
    {
        if (!socket_)
            return "InValid";

        return socket_->remote_endpoint().address().to_string();
    }

    uint16_t TcpSession::getRemotePort() const
    {
        if (!socket_)
            return 0;

        return socket_->remote_endpoint().port();
    }

    void TcpSession::start()
    {
        auto self(shared_from_this());
        BaseBufferPtr buffer_ptr = recieved_buffer_.allocBuff();

        auto read_cb = [this, &buffer_ptr, self](const ERR_CODE& ec, const std::size_t& size)
        {
            if (!ec && size)
            {
                buffer_ptr->consume_wr(size);
                onReceive(buffer_ptr);
                start();
            }
            else
            {
                if (ec.value() == boost::asio::error::eof)
                {
                    INFO("[TcpSession] session closed");
                    //if (disc_cb_)
                    //    disc_cb_(self);
                }
            }
        };

        socket_->async_receive(boost::asio::buffer(buffer_ptr->begin(), buffer_ptr->getCapacity()), strand_.wrap(read_cb));
    }

    void TcpSession::onReceive(const BaseBufferPtr& buf)
    {
        recieved_buffer_.emplace_back(buf);
        parseToMessage();
    }

    void TcpSession::onReceive(const char* buf, const uint32_t& size)
    {
        recieved_buffer_.writeData(buf, size);
        parseToMessage();
    }

    void TcpSession::sendMsg(Message&& msg)
    {
        sendMsgDirect(msg);
    }

    void TcpSession::sendMsg(const Message& msg)
    {
        sendMsgDirect(msg);
    }

    void TcpSession::sendMsgDirect(const Message& msg)
    {
        auto self(shared_from_this());
        socket_->async_send(boost::asio::buffer(msg.begin(), msg.size()),
            strand_.wrap(
                [this, self](const boost::system::error_code& ec, std::size_t size)
                {
                    if (!ec && size)
                    {
                        // net_analyze_->recordSend(getConnIdx(), size);
                    }
                    else
                    {
                        std::cout << "[sendMsgs()] faild\n";
                        ERR("[TcpSession][sendMsgDirect] send failed, addr={}, port={}", getRemoteAddr(),getRemotePort());
                    }
                }));
    }
    
    void TcpSession::parseToMessage()
    {
        while (1)
        {
            if (!parse_head_ok_)
            {
                // read head
                const uint32_t canReadSize = recieved_buffer_.canReadSize();
                if (canReadSize < HEADER_LEN)
                    break;

                uint32_t readed = recieved_buffer_.read(&cur_header_, HEADER_LEN);
                if (readed < HEADER_LEN)
                    break;
                
                parse_head_ok_ = true;
            }

            if (!parse_head_ok_)
                break;

            // read body
            const uint32_t canReadSize = recieved_buffer_.canReadSize();
            if (canReadSize < cur_header_.msg_len)
                break;

            Message* msg = new Message(cur_header_.msg_len);
            if (cur_header_.msg_len != recieved_buffer_.read(msg, cur_header_.msg_len))
                break;

            msg->setHeader(cur_header_);
            parse_head_ok_ = false;

#ifdef _MULTI_THREADS
            recived_msg_queue_.Enqueue(msg);
#else
            doMsg(msg);
#endif
        }
    }

    void TcpSession::processReceivedMsg()
    {
        while (true)
        {
            Message* msg = nullptr;
            if (!recived_msg_queue_.Dequeue(msg))
                break;

            if (!msg)
                continue;

            if (msg_callback_)
            {
                msg_callback_(*msg);
            }
            else
            {
                doMsg(*msg);
            }

            delete msg;
        }
    }

    void TcpSession::doMsg(const Message& msg)
    {
        doMsg(std::move(msg));
    }

    void TcpSession::doMsg(Message&& msg)
    {

    }
}  // namespace cncpp