#include <iostream>
#include <thread>
#include <chrono>
#include <string>

#define DWT_DEBUG
#include "Logger.h"

#include "InetAddress.h"

#include "Poller.h"
#include "EventLoop.h"

/**
 * 日志测试
*/
void Logger_test() {

    LOG_DEBUG("%d %s %s", 1024, "dwt", "这是debug日志");

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    LOG_INFO("%d %s %s", 1024, "dwt", "这是info日志");

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    LOG_WARN("%d %s %s", 1024, "dwt", "这是warn日志");

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    LOG_ERROR("%d %s %s", 1024, "dwt", "这是error日志");

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    LOG_FATAL("%d %s %s", 1024, "dwt", "这是fatal日志");
}

/**
 * InetAddress 测试
*/
void InetAddress_test() {
    dwt::InetAddress addr("127.0.0.1", 8888);

    LOG_DEBUG("IP: %s Port: %d", addr.toIp().c_str(), addr.toPort());
    LOG_DEBUG("IP_Port: %s", addr.toIpPort().c_str());
}



void Poller_test() {

    dwt::EventLoop loop;

    dwt::Poller::newDefaultPoller(&loop);

}

void EventLoop_test() {
    dwt::EventLoop loop;
}



int main() {

    // Logger_test();

    // InetAddress_test();

    // Poller_test();

    EventLoop_test();
    
    return 0;
}