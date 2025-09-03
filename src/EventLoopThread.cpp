#include "EventLoopThread.h"
#include "EventLoop.h"
#include "Logger.h"

namespace dwt {


#ifdef USE_COMPOSIZTION

EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const std::string& name)
    : loop_(nullptr)
    , m_exiting(false)
    , m_thread(std::bind(&EventLoopThread::threadFunc, this), name)
    , threadInitCallback_(cb)
    , m_mutex()
    , m_cond() {
    
    // 构造
}

EventLoopThread::~EventLoopThread() {

    m_exiting = true;
    if(loop_) {
        loop_->quit();

        m_thread.join();
    }

}

EventLoop* EventLoopThread::startLoop() {

    m_thread.start();   // 启动新线程, 执行EventLoopThread::threadFunc()函数
    
    EventLoop* loop = nullptr;

    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while(loop_ == nullptr) {
            m_cond.wait(lock);
        }
        loop = loop_;
    }
    return loop;
}


void EventLoopThread::threadFunc() {

    EventLoop loop; // 创建一个独立的loop
    
    if(threadInitCallback_) {            // 线程初始化完毕的回调
        threadInitCallback_(&loop);      // 执行ThreadInitCallback函数
    }

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        loop_ = &loop;

        m_cond.notify_one();    // 通知, m_loop已经和EventLoopThread绑定了
    }

    loop.loop();    // 开启事件循环 loop_->loop();
    

    
    std::lock_guard<std::mutex> lock(m_mutex);
    loop_ = nullptr;
}

#else


EventLoopThread::EventLoopThread(const ThreadInitCallback& cb, const std::string& name)
    : dwt::Thread(std::bind(&EventLoopThread::threadFunc, this), name)
    , loop_(nullptr)
    , m_exiting(false)
    , threadInitCallback_(cb)
    , m_mutex()
    , m_loop_created_promise() {
    
    // 构造
}

EventLoopThread::~EventLoopThread() {

    m_exiting = true;
    if(loop_) {
        loop_->quit();

        this->join();
    }

}

EventLoop* EventLoopThread::startLoop() {

    std::future<void> future = m_loop_created_promise.get_future();

    this->start();   // 启动新线程, 执行EventLoopThread::threadFunc()函数
    // threadFunc会创建loop，下面的代码要等待loop创建完毕才能返回
    

    future.wait();   // 等待loop创建完毕
    LOG_TRACE("EventLoopThread::startLoop() {} loop_ created", name_);

    assert(loop_ != nullptr);
    
    return loop_;
}


void EventLoopThread::threadFunc() {

    EventLoop loop; // 创建一个独立的loop
    // loop是在该线程栈上创建的

    LOG_TRACE("EventLoopThread::threadFunc() {} loop_ creating", name_);
    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    if(threadInitCallback_) {            // 线程初始化完毕的回调
        threadInitCallback_(&loop);      // 执行ThreadInitCallback函数
    }

    loop_ = &loop;
    m_loop_created_promise.set_value();   // 通知, loop创建完毕

    loop.loop();    // 开启事件循环 loop_->loop();
        
    std::lock_guard<std::mutex> lock(m_mutex);
    loop_ = nullptr;
}

#endif


}