#pragma once

#include <boost/asio.hpp>
#include <list>
#include <memory>
#include <vector>

#include "Singleton.h"
#include "NetDefine.h"


namespace cncpp
{
    class IoConnectPool : public Singleton<IoConnectPool>
	{
    public:
        /// Construct the io_context pool.
        explicit IoConnectPool();

        void init(const uint16_t pool_size);

        /// Run all io_context objects in the pool.
        void run();

        /// Stop all io_context objects in the pool.
        void stop();

        /// Get an io_context to use.
        IO_CONTEXT& get_io_context();

    private:
        IoConnectPool(const IoConnectPool&)            = delete;
        IoConnectPool& operator=(const IoConnectPool&) = delete;

        typedef std::shared_ptr<IO_CONTEXT> io_context_ptr;
        typedef boost::asio::executor_work_guard<boost::asio::io_context::executor_type>
            io_context_work;

        /// The pool of io_contexts.
        std::vector<io_context_ptr> io_contexts_;

        /// The work that keeps the io_contexts running.
        std::list<io_context_work> work_;

        /// The next io_context to use for a connection.
        uint16_t next_io_context_;

        uint16_t pool_size_ = 0;
    };
}

#define sIoContextPool cncpp::IoConnectPool::getMe()