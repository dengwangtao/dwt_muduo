#pragma once

#include "noncopyable.h"
#include <string>
#include "spdlog/spdlog.h"
#include "singleton.h"
#include <iostream>

namespace dwt {


class Logger: noncopyable, public Singleton<Logger> {

public:
    static void init(spdlog::level::level_enum min_level = spdlog::level::info)
    {
        spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] %v");
        spdlog::set_level(min_level);
        spdlog::flush_on(spdlog::level::err);
        spdlog::set_error_handler([](const std::string& msg) {
            std::cerr << "Log error: " << msg << std::endl;
        });
    }
};

#define G_LOGGER dwt::Logger::instance()


#define LOG_TRACE(logMsgFormat, ...) spdlog::trace(logMsgFormat, ##__VA_ARGS__)

#define LOG_DEBUG(logMsgFormat, ...) spdlog::debug(logMsgFormat, ##__VA_ARGS__)

#define LOG_INFO(logMsgFormat, ...) spdlog::info(logMsgFormat, ##__VA_ARGS__)

#define LOG_WARN(logMsgFormat, ...) spdlog::warn(logMsgFormat, ##__VA_ARGS__)

#define LOG_ERROR(logMsgFormat, ...) spdlog::error(logMsgFormat, ##__VA_ARGS__)

#define LOG_FATAL(logMsgFormat, ...) spdlog::critical(logMsgFormat, ##__VA_ARGS__)

} // namespace dwt