#pragma once

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <thread>

namespace cncpp
{
    class BoostThreadPool
    {
    public:
        explicit BoostThreadPool(std::size_t numThreads = std::thread::hardware_concurrency())
            : _impl(numThreads)
        {
        }

        template <typename T>
        decltype(auto) PostWork(T&& work)
        {
            return boost::asio::post(_impl, std::forward<T>(work));
        }

        void Join()
        {
            _impl.join();
        }

        void Stop()
        {
            _impl.stop();
        }

    private:
        boost::asio::thread_pool _impl;
    };
}