#include "Timestamp.h"

#include <chrono>
#include "fmt/format.h"

namespace dwt{

Timestamp::Timestamp()
    : time_s_(0)
{
}

Timestamp::Timestamp(s64 sec)
    : time_s_(sec)
{
}

// static
Timestamp Timestamp::now()
{
    using namespace std::chrono;

    auto now = system_clock::now();
    auto duration = now.time_since_epoch();
    auto ms = duration_cast<seconds>(duration).count();
    return Timestamp(ms);
}

std::string Timestamp::toString() const
{
    using namespace std::chrono;

    // 将秒转换为时间点
    system_clock::time_point tp = system_clock::from_time_t(time_s_);
    
    // 将 time_point 转换为 tm 结构
    std::time_t time = system_clock::to_time_t(tp);
    std::tm* tm_time = std::localtime(&time);

    // 使用 fmt::format (C++20)
    return fmt::format("{:04}/{:02}/{:02} {:02}:{:02}:{:02}",
                        tm_time->tm_year + 1900, tm_time->tm_mon + 1, tm_time->tm_mday,
                        tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);
}


}