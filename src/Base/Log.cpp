#include "Log.h"

namespace cncpp
{
    void TinyLogger::init()
    {
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        //console_sink->set_pattern("[%Y-%m-%d %H:%M:%S] [%l][%t] %v");

        auto hour_file_sink = std::make_shared<cncpp::MyHourlyFileSink_mt>("../../log/hourly_log");
        //auto hour_file_sink = std::make_shared<cncpp::MyHourlyFileSink_mt>("hourly_log");
        hour_file_sink->set_level(spdlog::level::trace);
        //hour_file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l][%t] %v");

        tinylogger = std::make_shared<spdlog::logger>(
            "", std::initializer_list<spdlog::sink_ptr>{ console_sink, hour_file_sink });
        tinylogger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l][%t] %v");
    }
}