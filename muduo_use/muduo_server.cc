#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>

#include <iostream>
#include <string>


class DwtServer {

public:
    DwtServer (
        muduo::net::EventLoop* loop,
        const muduo::net::InetAddress& listenAddr,
        const std::string& nameArg)
        :m_server(loop, listenAddr, nameArg), loop_(loop)  {

        // 构造函数

        // 绑定 连接到达回调
        m_server.setConnectionCallback(std::bind(&DwtServer::onConnection, this, std::placeholders::_1));

        // 绑定 数据到达回调
        m_server.setMessageCallback(std::bind(&DwtServer::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

        // 设置线程数
        m_server.setThreadNum(4);
    }

    void start() {
        // 开启事件循环
        m_server.start();
    }

private:
    void onConnection(const muduo::net::TcpConnectionPtr& conn) {
        if(conn->connected()) {
            std::cout << "New connection " << conn->peerAddress().toIpPort() << std::endl;
        } else {
            std::cout << "Connection " << conn->peerAddress().toIpPort() << " Offline" << std::endl;
            conn->shutdown();
        }
    }

    void onMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buffer, muduo::Timestamp time) {
        std::string buf = buffer->retrieveAllAsString();
        std::cout << "receive data from " << conn->peerAddress().toIpPort() << ": " << buf << std::endl;
        conn->send(buf);
    }

private:

    muduo::net::EventLoop *loop_;        // 事件循环
    muduo::net::TcpServer m_server;

};

int main() {

    muduo::net::EventLoop loop;
    muduo::net::InetAddress addr("127.0.0.1", 8888);

    DwtServer server(&loop, addr, "myserver");

    server.start();


    // 开启事件循环
    loop.loop();

    return 0;
}

// g++ muduo_server.cc -o server -lmuduo_net -lmuduo_base -lpthread