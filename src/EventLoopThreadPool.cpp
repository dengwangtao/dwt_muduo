#include "EventLoopThreadPool.h"

#include "EventLoopThread.h"

#include "Logger.h"

namespace dwt {



EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const std::string& name)
    : baseLoop_(baseLoop)
    , name_(name)
    , started_(false)
    , numThreads_(0)
    , next_(0) {
    
    LOG_INFO("EventLoopThreadPool[{}] created", name_);
}
    
EventLoopThreadPool::~EventLoopThreadPool() {
    // nothing
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb) {

    started_ = true;

    // baseLoop_ 放到loops_的第一个位置
    loops_.push_back(baseLoop_);

    // 创建线程
    for(int i = 0; i < numThreads_; ++ i)
    {
        std::string sub_name = fmt::format("EventLoopThread_{}-{}", name_, i);
        {
            auto elt = std::make_unique<EventLoopThread>(cb, sub_name);
            threads_.push_back(std::move(elt));
        }

        auto* loop = threads_.back()->startLoop(); // 获取该线程所对应的EventLoop
        loops_.push_back(loop);
    }

    // 如果没有创建线程，则在主线程中运行， cb为用户自定义的初始化线程的回调
    if(numThreads_ == 0 && cb)
    {
        cb(baseLoop_);
    }
}


// 轮询的方式给分channel给subloop
EventLoop* EventLoopThreadPool::getNextLoop() {

    // 默认为主线程
    EventLoop* loop = nullptr;
    if (loops_.size() == 1)
    {
        return baseLoop_;
    }

    // 如果有工作线程，则轮询
    if(!loops_.empty())
    {
        // round-robin
        // 如果要轮巡. 就不算0
        if (next_ == 0) next_ = 1;
        loop = loops_[next_ ++];
        next_ %= loops_.size();
    }

    return loop;
}


}