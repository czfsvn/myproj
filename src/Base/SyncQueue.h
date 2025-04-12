#pragma once


#include <list>
#include <mutex>
#include <condition_variable>
#include <iostream>

using namespace std;

namespace cncpp
{
    template <typename T>
    class SyncQueueBlocking
    {
    public:
        SyncQueueBlocking() {}
        ~SyncQueueBlocking()
        {
            data_list_.clear();
        }

        void stop()
        {
            {
                std::lock_guard<std::mutex> lk(mutex_);
                need_stop_ = true;
            }
            not_full_condition_.notify_all();
            not_empty_condition_.notify_all();
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lk(mutex_);
            return data_list_.empty();
        }

        size_t size()
        {
            std::lock_guard<std::mutex> lk(mutex_);
            return data_list_.size();
        }

        void put(T&& data)
        {
            add(std::forward<T>(data));
        }

        void put(const T& data)
        {
            add(data);
        }

        void take(std::list<T>& tlist)
        {
            std::unique_lock<std::mutex> lk(mutex_);
            not_empty_condition_.wait(lk,
                [this]
                {
                    return need_stop_ || notEmpty();
                });
            if (need_stop_)
                return;

            tlist = std::move(data_list_);
            not_full_condition_.notify_one();
        }

        void take(T& data)
        {
            std::unique_lock<std::mutex> lk(mutex_);
            not_empty_condition_.wait(lk,
                [this]
                {
                    return need_stop_ || notEmpty();
                });
            if (need_stop_)
                return;

            data = std::move(data_list_.front());
            data_list_.pop_front();
            not_full_condition_.notify_one();
        }

    private:
        bool notEmpty() const
        {
            bool empty = data_list_.empty();
            if (empty)
                std::cout << "queue is empty : " << std::this_thread::get_id() << std::endl;

            return !empty;
        }

        template <typename F>
        void add(F&& data)
        {
            std::unique_lock<std::mutex> lk(mutex_);
            not_full_condition_.wait(lk,
                [this]
                {
                    return need_stop_;
                });

            if (need_stop_)
                return;

            data_list_.emplace_back(std::forward<F>(data));
            not_empty_condition_.notify_one();
        }

    private:
        bool                    need_stop_ = false;
        std::list<T>            data_list_;
        std::mutex              mutex_;
        std::condition_variable not_full_condition_;
        std::condition_variable not_empty_condition_;
    };

    template <typename T>
    class SyncQueue
    {
    public:
        SyncQueue() {}
        ~SyncQueue()
        {
            data_list_.clear();
        }

        void stop()
        {
            std::lock_guard<std::mutex> lk(mutex_);
            need_stop_ = true;
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lk(mutex_);
            return data_list_.empty();
        }

        size_t size()
        {
            std::lock_guard<std::mutex> lk(mutex_);
            return data_list_.size();
        }

        void put(T&& data)
        {
            add(std::forward<T>(data));
        }

        void put(const T& data)
        {
            add(data);
        }

        void take(std::list<T>& tlist)
        {
            std::unique_lock<std::mutex> lk(mutex_);
            tlist = std::move(data_list_);
        }

        void take(T& data)
        {
            std::unique_lock<std::mutex> lk(mutex_);
            data = std::move(data_list_.front());
            data_list_.pop_front();
        }

    private:
        bool notEmpty() const
        {
            bool empty = data_list_.empty();
            if (empty)
                std::cout << "queue is empty : " << std::this_thread::get_id() << std::endl;

            return !empty;
        }

        template <typename F>
        void add(F&& data)
        {
            if (need_stop_)
                return;

            {
                std::unique_lock<std::mutex> lk(mutex_);
                data_list_.emplace_back(std::forward<F>(data));
            }
        }

    private:
        bool         need_stop_ = false;
        std::list<T> data_list_;
        std::mutex   mutex_;
    };
}  // namespace cncpp