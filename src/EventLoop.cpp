#include "EventLoop.h"

#include <sys/eventfd.h>
#include <unistd.h>

#include "Logger.h"
#include "Poller.h"
#include "Channel.h"

namespace dwt {

// 方式一个线程创建多个EventLoop
__thread EventLoop* t_loopInThisThread = nullptr;

const int kPollTimeMs = 10000;

int createEventFd() {
    int evfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if(evfd < 0)  {
        LOG_FATAL("create eventfd error");
    }
    return evfd;
}


EventLoop::EventLoop()
    :m_looping(false),
    m_quit(false),
    m_callingPendingFunctors(false),
    m_threadId(CurrentThread::tid()),
    m_poller(Poller::newDefaultPoller(this)),
    m_wakeupFd(createEventFd()),
    m_wakeupChannel(new Channel(this, m_wakeupFd)),
    m_currentActiveChannel(nullptr) {

    //
    LOG_DEBUG("EventLoop created %p in thread %d", this, m_threadId);

    if(t_loopInThisThread != nullptr) {
        LOG_FATAL("another eventloop %p exists in this thread %d", t_loopInThisThread, m_threadId);
    } else {
        t_loopInThisThread = this;
    }

    // 设置wakeupfd的事件类型和回调操作
    m_wakeupChannel->setReadCallback(
        std::bind(&EventLoop::handleRead, this)
    );
    m_wakeupChannel->enableReading();   // 主reactor监视this的EPOLLIN事件
}

EventLoop::~EventLoop() {
    m_wakeupChannel->disableAll();
    m_wakeupChannel->remove();
    ::close(m_wakeupFd);
    t_loopInThisThread = nullptr;
}


void EventLoop::loop() {
    
}

void EventLoop::quit() {

}




void EventLoop::handleRead() {

    uint64_t one = 1;

    ssize_t n = ::read(m_wakeupFd, &one, sizeof one);

    if(n != sizeof one) {
        LOG_ERROR("EventLoop::handleRead() reads %ld bytes instead of 8", n);
    }

}




void EventLoop::updateChannel(Channel* channel) {
    m_poller->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel) {
    m_poller->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel* channel) {
    return m_poller->hasChannel(channel);
}


}