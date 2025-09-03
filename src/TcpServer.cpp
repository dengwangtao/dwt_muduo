#include "TcpServer.h"

#include "Logger.h"
#include "TcpConnection.h"

#include <string.h>

namespace dwt {

static EventLoop* checkLoopNotNull(EventLoop* loop) {
    if(loop == nullptr) {
        LOG_FATAL("mainLoop is nullptr");
    }
    return loop;
}

TcpServer::TcpServer(
    EventLoop* loop, const InetAddress& listenAddr,
    const std::string& name, Option option)
        : loop_(checkLoopNotNull(loop))
        , ipPort_(listenAddr.toIpPort())
        , name_(name)
        , acceptor_(std::make_unique<Acceptor>(loop, listenAddr, option == Option::kReusePort))
        , threadPool_(std::make_shared<EventLoopThreadPool>(loop, name))
        , connectionCallback_()
        , messageCallback_()
        , started_(0)
        , nextConnId_(1) {
    
    // 构造

    acceptor_->setNewConnectionCallback(
        std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2)
    );


}
TcpServer::~TcpServer() {
    for(auto& item : connections_) {
        TcpConnectionPtr conn(item.second);

        item.second.reset(); // 让item.second不再指向资源, 只让conn指向资源

        conn->getLoop()->queueInLoop(
            std::bind(&TcpConnection::connectDestoryed, conn)
        );
    }
}


void TcpServer::setThreadNum(int numThreads) {
    threadPool_->setThreadNum(numThreads);
}

void TcpServer::start() {
    if(started_ ++ == 0) {
        threadPool_->start(threadInitCallback_);

        loop_->runInLoop(
            std::bind(&Acceptor::listen, acceptor_.get())
        );
    }
}



void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr) {

    EventLoop* ioLoop = threadPool_->getNextLoop();

    std::string connName = ipPort_ + "#" + std::to_string(nextConnId_);
    ++ nextConnId_;

    LOG_DEBUG("TcpServer::newConnection: {}", connName);

    sockaddr_in local;
    ::memset(&local, 0, sizeof local);
    socklen_t socklen = static_cast<socklen_t>(sizeof local);
    if(::getsockname(sockfd, (sockaddr*)&local, &socklen) < 0) {
        LOG_ERROR("TcpServer::newConnection getsockname error");
    }

    InetAddress localAddr(local);

    TcpConnectionPtr conn = std::make_shared<TcpConnection>(ioLoop, connName, sockfd, localAddr, peerAddr);

    connections_[connName] = conn;

    conn->setConnectionCallback(connectionCallback_);
    conn->setMessageCallback(messageCallback_);
    conn->setWriteCompleteCallback(writeCompleteCallback_);
    conn->setCloseCallback(
        std::bind(&TcpServer::removeConnection, this, std::placeholders::_1)
    );
    
    ioLoop->runInLoop(
        std::bind(&TcpConnection::connectEstablished, conn)
    );
    // ioLoop->wakeup();

}

void TcpServer::removeConnection(const TcpConnectionPtr& conn) {

    loop_->runInLoop(
        std::bind(&TcpServer::removeConnectionInLoop, this, conn)
    );

}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn) {

    std::string connName = conn->name();

    LOG_INFO("TcpServer::removeConnectionInLoop [{}] connection[{}]", name_, connName);

    connections_.erase(connName);

    EventLoop* ioLoop = conn->getLoop();
    ioLoop->queueInLoop(
        std::bind(&TcpConnection::connectDestoryed, conn)
    );
}


}