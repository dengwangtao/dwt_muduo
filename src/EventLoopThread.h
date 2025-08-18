#pragma once

#include "noncopyable.h"

#include <functional>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <future>

#include "Thread.h"

namespace dwt {

// 前置声明
class EventLoop;


#define USE_COMPOSIZTION
#undef USE_COMPOSIZTION

#ifdef USE_COMPOSIZTION // 组合模式

class EventLoopThread : noncopyable {

public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(), const std::string& name = std::string());

    ~EventLoopThread();

    EventLoop* startLoop();

private:

    void threadFunc();


    EventLoop* m_loop;

    bool m_exiting;

    dwt::Thread m_thread;

    ThreadInitCallback m_threadInitCallback;

    std::mutex m_mutex;
    std::condition_variable m_cond;
};

#else // 继承模式

class EventLoopThread : public dwt::Thread
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;

    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(), const std::string& name = std::string());

    ~EventLoopThread();

    EventLoop* startLoop();

private:

    /**
     * @brief 线程函数
     * @note 线程函数，在线程中运行
     * @return void
     */
    void threadFunc();

    EventLoop* m_loop;

    bool m_exiting;

    ThreadInitCallback m_threadInitCallback;

    std::mutex m_mutex;
    std::promise<void> m_loop_created_promise;
};


#endif


}