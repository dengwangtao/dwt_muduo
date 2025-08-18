#include "EventLoopThread.h"
#include "EventLoop.h"
#include "Logger.h"

namespace dwt {


#ifdef USE_COMPOSIZTION

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const std::string& name)
    : m_loop(nullptr)
    , m_exiting(false)
    , m_thread(std::bind(&EventLoopThread::threadFunc, this), name)
    , m_threadInitCallback(cb)
    , m_mutex()
    , m_cond() {
    
    // 构造
}

EventLoopThread::~EventLoopThread() {

    m_exiting = true;
    if(m_loop) {
        m_loop->quit();

        m_thread.join();
    }

}

EventLoop* EventLoopThread::startLoop() {

    m_thread.start();   // 启动新线程, 执行EventLoopThread::threadFunc()函数
    
    EventLoop* loop = nullptr;

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while(m_loop == nullptr) {
            m_cond.wait(lock);
        }
        loop = m_loop;
    }
    return loop;
}


void EventLoopThread::threadFunc() {

    EventLoop loop; // 创建一个独立的loop
    
    if(m_threadInitCallback) {            // 线程初始化完毕的回调
        m_threadInitCallback(&loop);      // 执行ThreadInitCallback函数
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_loop = &loop;

        m_cond.notify_one();    // 通知, m_loop已经和EventLoopThread绑定了
    }

    loop.loop();    // 开启事件循环 m_loop->loop();
    

    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_loop = nullptr;
}

#else


EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const std::string& name)
    : dwt::Thread(std::bind(&EventLoopThread::threadFunc, this), name)
    , m_loop(nullptr)
    , m_exiting(false)
    , m_threadInitCallback(cb)
    , m_mutex()
    , m_loop_created_promise() {
    
    // 构造
}

EventLoopThread::~EventLoopThread() {

    m_exiting = true;
    if(m_loop) {
        m_loop->quit();

        this->join();
    }

}

EventLoop* EventLoopThread::startLoop() {

    std::future<void> future = m_loop_created_promise.get_future();

    this->start();   // 启动新线程, 执行EventLoopThread::threadFunc()函数
    // threadFunc会创建loop，下面的代码要等待loop创建完毕才能返回
    

    future.wait();   // 等待loop创建完毕
    LOG_TRACE("EventLoopThread::startLoop() {} m_loop created", m_name);
    
    return m_loop;
}


void EventLoopThread::threadFunc() {

    EventLoop loop; // 创建一个独立的loop
    // loop是在该线程栈上创建的

    LOG_TRACE("EventLoopThread::threadFunc() {} m_loop creating", m_name);
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    if(m_threadInitCallback) {            // 线程初始化完毕的回调
        m_threadInitCallback(&loop);      // 执行ThreadInitCallback函数
    }

    m_loop_created_promise.set_value();   // 通知, loop创建完毕

    loop.loop();    // 开启事件循环 m_loop->loop();
        
    std::lock_guard<std::mutex> lock(m_mutex);
    m_loop = nullptr;
}

#endif


}