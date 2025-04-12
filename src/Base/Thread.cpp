#include "Thread.h"
#include <functional>
#include "Misc.h"

namespace cncpp
{
    Thread::Thread()
    {
        stopped_.store(true);
    }

    Thread::~Thread() {}

    // base option
    void Thread::start()
    {
        stopped_ = false;
        if (!thrd_ptr_)
        {
            thrd_ptr_ =
                std::unique_ptr<std::thread>(new std::thread(std::bind(&Thread::thrdloop, this)));
        }
    }

    void Thread::stop()
    {
        stopped_ = true;
    }

    void Thread::join()
    {
        if (thrd_ptr_ && thrd_ptr_->joinable())
        {
            thrd_ptr_->join();
        }
    }

    void Thread::thrdloop()
    {
        while (!isStop())
        {
            run();
            cncpp::sleepfor_microseconds(5);
        }
    }

    bool Thread::isStop()
    {
        return stopped_;
    }
}  // namespace cncpp
