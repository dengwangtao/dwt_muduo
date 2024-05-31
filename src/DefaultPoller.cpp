#include "Poller.h"
#include "EpollPoller.h"
#include "stdlib.h"

namespace dwt {

// TODO: 
Poller* Poller::newDefaultPoller(EventLoop* loop) {
    
    if(::getenv("MUDUO_USE_POLL")) {
        // 生成poll实例
    } else {
        // 生成epoll实例
        return new EpollPoller(loop);
    }

    return nullptr;
}


}