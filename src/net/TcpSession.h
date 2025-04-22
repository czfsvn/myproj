#pragma once

#include "NetDefine.h"
#include "Buffer.h"
#include "MPSCQueue.h"

namespace cncpp
{
    enum class SessionState : uint16_t
    {
        eState_inValid = 0,  // 无效
        eState_verify  = 1,  // 待验证连接
        eState_ok      = 2,  // 验证Ok
    };

    class TcpSession : public std::enable_shared_from_this<TcpSession>
    {
    public:
        TcpSession(IO_CONTEXT& ioc, const SOCKET_PTR sockptr);

        void setMsgCallback(const MessageCallBack& cb);

        void start();

        bool isOkay() const;
        bool isVerifying() const;
        bool isInValid() const;

        std::string getRemoteAddr() const;
        uint16_t    getRemotePort() const;

        SOCKET_PTR getSocket() const;

        void onReceive(const BaseBufferPtr& buf);
        void onReceive(const char* buf, const uint32_t& size);

        void processReceivedMsg();

        void sendMsg(Message&& msg);
        void sendMsg(const Message& msg);
        void sendMsgDirect(const Message& msg);

    private:
        void parseToMessage();

        void doMsg(const Message& msg);
        void doMsg(Message&& msg);


    private:
        SessionState                    state_  = SessionState::eState_inValid;
        SOCKET_PTR                      socket_ = {};
        boost::asio::io_context::strand strand_;

        RingBuffer recieved_buffer_ = {};

        MPSCQueueNonIntrusive<Message> recived_msg_queue_ = {};
        //MPSCQueueNonIntrusive<Message> send_msg_queue_    = {};

        bool parse_head_ok_ = false;
        MsgHeader cur_header_    = {};

        MessageCallBack msg_callback_ = {};

    };
}