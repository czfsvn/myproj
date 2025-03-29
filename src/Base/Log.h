#pragma once


#include <iomanip>
#include <fstream>
#include <sstream>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/base_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Singleton.h"
#include "TimeUtils.h"

#define SPDLOG

#ifdef SPDLOG

#define TRACE    cncpp::TinyLogger::getMe().log()->trace     // 0
#define DEBUG    cncpp::TinyLogger::getMe().log()->debug     // 1
#define INFO     cncpp::TinyLogger::getMe().log()->info      // 2
#define WARN     cncpp::TinyLogger::getMe().log()->warn      // 3
#define ERR      cncpp::TinyLogger::getMe().log()->error     // 4
#define CRITICAL cncpp::TinyLogger::getMe().log()->critical  // 5

#else

#define TRACE(fmt, ...)
#define DEBUG(fmt, ...)
#define INFO(fmt, ...)
#define WARN(fmt, ...)
#define ERR(fmt, ...)
#define CRITICAL(fmt, ...)

#endif

namespace cncpp
{
    template <typename Mutext>
    class MyHourlyFileSink : public spdlog::sinks::base_sink<Mutext>
    {
    public:
        MyHourlyFileSink(const std::string& base_filename, int flush_interval_seconds = 3600)
            : base_filename_(base_filename), flush_interval_(flush_interval_seconds)
        {
            open_file();
        }

        ~MyHourlyFileSink() {}

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override
        {
            uint32_t now = time(NULL);
            // if (now >= next_check_time_)  // todo: some bugs accured
            {
                uint32_t now_hour = TimeUtils::getLocalHour(now);
                if (now_hour != current_hour_)
                {
                    open_file();
                }
            }

            spdlog::memory_buf_t formatted;
            spdlog::sinks::base_sink<Mutext>::formatter_->format(msg, formatted);

            file_.write(formatted.data(), formatted.size());

            //std::string logMessage(formatted.begin(), formatted.end());
            //file_ << logMessage;// << std::endl;
            //file_.flush();
        }

        void flush_() override
        {
            file_.flush();
        }

    private:
        void open_file()
        {
            time_t             now = TimeUtils::getRealTimeSec();// time(NULL);
            std::tm tm_time;
            TimeUtils::getLocalTime(tm_time, now);
            // std::ostringstream oss;
            /// oss << base_filename_ << "_" << std::put_time(&tm_time, "%Y-%m-%d_%H") << ".log";
            // const std::string data_str = oss.str();
            // const std::string& new_filename = oss.str();
            std::string new_filename = base_filename_ + "_ssss.log";
            //std::string new_filename = "hour.log";

            file_.close();
            file_.open(new_filename.c_str(), std::ios::app | std::ios::out);

            if (!file_.is_open())
            {
                std::cerr << "Failed to open file: " << new_filename << std::endl;
                return;
            }

            current_hour_ = tm_time.tm_hour;

            // 下次检车切换文件的时间，且提前10秒开始检查
            // todo: checkbugs
            next_check_time_ = TimeUtils::getLocalDayZero(now)
                + (current_hour_ + 1) * TimeUtils::kHourSeconds - 10;
        }

    private:
        std::string   base_filename_ = "hours";
        std::ofstream file_;
        uint32_t      flush_interval_ = 0;  // 刷新间隔，单位为秒
        uint32_t      next_check_time_ = 0;  // 下一次检查时间
        uint32_t      current_hour_ = 0;  // 当前小时
    };

    using MyHourlyFileSink_mt = MyHourlyFileSink<std::mutex>;
    using MyHourlyFileSink_st = MyHourlyFileSink<spdlog::details::null_mutex>;

    class TinyLogger : public Singleton<TinyLogger>
    {
    public:
        TinyLogger()
        {
            init();
        }

        ~TinyLogger() {}

        void init()
        {
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            console_sink->set_level(spdlog::level::warn);
            //console_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%l][%t] %v");

            auto hour_file_sink = std::make_shared<cncpp::MyHourlyFileSink_mt>("../../log/hourly_log");
            //auto hour_file_sink = std::make_shared<cncpp::MyHourlyFileSink_mt>("hourly_log");
            hour_file_sink->set_level(spdlog::level::trace);
            //hour_file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l][%t] %v");

            tinylogger = std::make_shared<spdlog::logger>(
                "", std::initializer_list<spdlog::sink_ptr>{ console_sink, hour_file_sink });
            tinylogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l][%t] %v");
        }

        std::shared_ptr<spdlog::logger> log()
        {
            return tinylogger;
        }

    private:
        std::shared_ptr<spdlog::logger> tinylogger = nullptr;
    };
}  // namespace cncpp