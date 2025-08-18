#include "EventLoopThreadPool.h"

#include "EventLoopThread.h"

#include "Logger.h"

namespace dwt {



EventLoopThreadPool::EventLoopThreadPool(EventLoop* baseLoop, const std::string& name)
    : m_baseLoop(baseLoop)
    , m_name(name)
    , m_started(false)
    , m_numThreads(0)
    , m_next(0) {
    
    LOG_INFO("EventLoopThreadPool[{}] created", m_name);
}
    
EventLoopThreadPool::~EventLoopThreadPool() {
    // nothing
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb) {

    m_started = true;

    // 创建线程
    for(int i = 0; i < m_numThreads; ++ i) {
        std::string sub_name = m_name + std::to_string(i);

        {
            auto elt = std::make_unique<EventLoopThread>(cb, sub_name);
            m_threads.push_back(std::move(elt));
        }

        auto* loop = m_threads.back()->startLoop(); // 获取该线程所对应的EventLoop
        m_loops.push_back(loop);
    }

    // 如果没有创建线程，则在主线程中运行， cb为用户自定义的初始化线程的回调
    if(m_numThreads == 0 && cb) {
        cb(m_baseLoop);
    }
}


// 轮询的方式给分channel给subloop
EventLoop* EventLoopThreadPool::getNextLoop() {

    // 默认为主线程
    EventLoop* loop = m_baseLoop;

    // 如果有工作线程，则轮询
    if(!m_loops.empty()) {
        // round-robin
        loop = m_loops[m_next ++];
        // if(static_cast<ssize_t>(m_next) >= m_loops.size()) {
        //     m_next = 0;
        // }
        m_next %= m_loops.size();
    }

    return loop;
}

std::vector<EventLoop*> EventLoopThreadPool::getAllLoops() {
    if(m_numThreads <= 0) {
        return {m_baseLoop};
    }

    return m_loops;
}


}