#pragma once

#include "NetDefine.h"
#include "Singleton.h"

namespace cncpp
{
    class AsioTimer : public Singleton<AsioTimer>
    {
    public:
        void init(IO_CONTEXT* ioc, const uint32_t ms)
        {
            interval_ = std::chrono::milliseconds(ms);
            timer_    = std::make_shared<boost::asio::steady_timer>(*ioc);
        }

        void setCallBack(const cncpp::TimerCallBack& cb)
        {
            callback_ = cb;
        }

        void start()
        {
            auto timer_lbc = [this](auto ec)
            {
                if (ec)
                    return;

                if (callback_)
                    callback_();

                start();
            };

            timer_->expires_after(interval_);
            timer_->async_wait(timer_lbc);
        }

    private:
        std::shared_ptr<boost::asio::steady_timer> timer_    = {};
        std::chrono::milliseconds                  interval_ = {};
        cncpp::TimerCallBack                       callback_ = {};
    };
}

#define sAsioTimer cncpp::AsioTimer::getMe()