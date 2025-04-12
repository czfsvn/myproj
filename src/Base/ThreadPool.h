#pragma once

#include "SyncQueue.h"
#include "Thread.h"
#include <vector>

namespace cncpp
{
    class BaseTask;
    class TaskPool;
    class TaskThread;
    using TaskPtr       = std::shared_ptr<BaseTask>;
    using TaskPoolPtr   = std::shared_ptr<TaskPool>;
    using TaskThreadPtr = std::shared_ptr<TaskThread>;

    class BaseTask
    {
    public:
        virtual bool work() = 0;
        virtual bool done() = 0;

        uint64_t taskid = 0;
    };

    class TaskThread : public Thread
    {
    public:
        TaskThread(const TaskPoolPtr& ptr);
        bool addTask(const TaskPtr& task);
        void run() override;

    private:
        TaskPoolPtr        pool_ptr_   = nullptr;
        SyncQueue<TaskPtr> task_queue_ = {};
    };

    class TaskPool : public std::enable_shared_from_this<TaskPool>
    {
    public:
        TaskPool(uint16_t size);

        std::shared_ptr<TaskPool> getShared()
        {
            return shared_from_this();
        }

        bool init();

        void addDone(TaskPtr& task);

        bool addNewTask(const TaskPtr& task);

        bool addNewTask(BaseTask* task);

        void runInMain();

        void joinAll();

        void startAll();

    private:
        TaskThreadPtr getThread();

    private:
        std::atomic<uint32_t>      cur_thread_idx_;  // = 0;
        uint16_t                   thread_size_ = 0;
        std::vector<TaskThreadPtr> thread_vec_  = {};
        SyncQueue<TaskPtr>         task_done_   = {};
    };
}  // namespace cncpp