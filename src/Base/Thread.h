#pragma once

#include <atomic>
#include <memory>
#include <thread>

namespace cncpp
{
    class Thread
    {
    public:
        Thread();
        virtual ~Thread();

        // base option
        virtual void start();

        virtual void stop();

        virtual void join();

        virtual void thrdloop();

        virtual bool isStop();

        // TO DO
        virtual void run() = 0;

    protected:
        Thread(const Thread&)            = delete;
        Thread& operator=(const Thread&) = delete;

    protected:
        std::atomic_bool             stopped_;
        std::unique_ptr<std::thread> thrd_ptr_ = nullptr;
    };
}  // namespace cncpp