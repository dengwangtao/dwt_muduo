#pragma once

#include "noncopyable.h"
#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "Callbacks.h"
#include "TcpConnection.h"

// 统一包含
#include "Logger.h"
#include "Poller.h"
#include "Thread.h"


#include <functional>
#include <memory>
#include <string>
#include <atomic>
#include <unordered_map>


namespace dwt {


/**
 * TcpServer
 */
class TcpServer : noncopyable {

public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    enum class Option {
        kNoReusePort,
        kReusePort
    };

    TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string& name, Option option = Option::kNoReusePort);
    ~TcpServer();

    const std::string& ipPort() const { return ipPort_; }
    const std::string& name() const { return name_; }
    EventLoop* getLoop() const { return loop_; }

    void setThreadInitCallback(const ThreadInitCallback& cb) { threadInitCallback_ = cb; }

    void setThreadNum(int numThreads);

    void start();

    void setConnectionCallback(const ConnectionCallback& cb) { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb) { messageCallback_ = cb; }

    // void setCloseCallback(const CloseCallback& cb) { m_closeCallback = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { writeCompleteCallback_ = cb; }

private:

    void newConnection(int sockfd, const InetAddress& peerAddr);

    void removeConnection(const TcpConnectionPtr& conn);

    void removeConnectionInLoop(const TcpConnectionPtr& conn);


    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;


    EventLoop* loop_;          // baseLoop, 用户传入
    const std::string ipPort_;
    const std::string name_;

    std::unique_ptr<Acceptor> acceptor_;   // 运行在 mainLoop, 监听新连接事件
    std::shared_ptr<EventLoopThreadPool> threadPool_;  // one loop per thread

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    // CloseCallback m_closeCallback;
    WriteCompleteCallback writeCompleteCallback_;
    
    ThreadInitCallback threadInitCallback_;

    std::atomic<int> started_;

    ConnectionMap connections_;    //保存所有连接

    int nextConnId_;

};


}