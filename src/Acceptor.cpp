#include "Acceptor.h"
#include "Logger.h"
#include "InetAddress.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


namespace dwt {

/**
 * 创建非阻塞的 listenfd
 */
static int createNonBlockingFd() {
    int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if(sockfd == -1) {
        LOG_FATAL("{}:{} {} error: {}",__FILE__, __LINE__, __FUNCTION__, errno);
    }
    return sockfd;
}



Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reusePort)
    : loop_(loop)
    , acceptSocket_(createNonBlockingFd())
    , acceptChannel_(loop, acceptSocket_.fd())
    , listening_(false) {

    // 构造
    acceptSocket_.setReuseAddr(true);
    acceptSocket_.setReusePort(reusePort);
    acceptSocket_.bindAddress(listenAddr);
    acceptChannel_.setReadCallback(
        std::bind(&Acceptor::handleRead, this)
    );
}

Acceptor::~Acceptor() {
    acceptChannel_.disableAll();
    acceptChannel_.remove();
}


void Acceptor::listen() {
    listening_ = true;
    acceptSocket_.listen();
    acceptChannel_.enableReading();
}


/**
 * 新连接到达
*/
void Acceptor::handleRead() {

    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);

    LOG_DEBUG("new connection from {}", peerAddr.toIpPort());

    if(connfd >= 0)
    {
        if(newConnectionCallback_)
        {
            // m_newConnectionCallback的功能: 轮询找到 subloop, 并唤醒 subloop, 分发客户端的channel
            newConnectionCallback_(connfd, peerAddr);
        }
        else
        {
            ::close(connfd);
        }
    }
    else
    {
        if(errno == EMFILE)
        {
            LOG_ERROR("error: socket reached limited");
        }
        else
        {
            LOG_ERROR("error: {}", errno);
        }
    }
}


}