#pragma once

#include "noncopyable.h"

#include <vector>
#include <memory>
#include <functional>
#include <string>

namespace dwt {


// 前置声明
class EventLoop;
class EventLoopThread;


class EventLoopThreadPool : noncopyable {

public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThreadPool(EventLoop* baseLoop, const std::string& name);
    
    ~EventLoopThreadPool();

    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    void setThreadNum(int numThreads) {numThreads_ = numThreads; }

    EventLoop* getNextLoop();   // 轮询的方式给分channel给subloop

    bool started() const { return started_; }

    const std::string& name() const { return name_; }

private:


    EventLoop* baseLoop_;          // 返回给用户

    std::string name_;
    bool started_;
    int numThreads_; // 工作线程数，默认为0，表示不创建新线程，只在主线程中运行
    int next_;

    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};





}