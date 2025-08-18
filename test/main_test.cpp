#define DWT_DEBUG

#include "TcpServer.h"

#include <iostream>
#include <thread>
#include <chrono>
#include <string>

/**
 * 日志测试
*/
void Logger_test() {

    LOG_DEBUG("{} {} {}", 1024, "dwt", "这是debug日志");

    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    LOG_INFO("{} {} {}", 1024, "dwt", "这是info日志");

    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    LOG_WARN("{} {} {}", 1024, "dwt", "这是warn日志");

    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    LOG_ERROR("{} {} {}", 1024, "dwt", "这是error日志");

    // std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    LOG_FATAL("{} {} {}", 1024, "dwt", "这是fatal日志");
}

/**
 * InetAddress 测试
*/
void InetAddress_test() {
    dwt::InetAddress addr("127.0.0.1", 8888);

    LOG_DEBUG("IP: {} Port: {}", addr.toIp().c_str(), addr.toPort());
    LOG_DEBUG("IP_Port: {}", addr.toIpPort().c_str());
}



void Poller_test() {

    dwt::EventLoop loop;

    dwt::Poller::newDefaultPoller(&loop);

}

void EventLoop_test() {
    dwt::EventLoop loop;
}


void Thread_test() {
    dwt::Thread t([]() {
        for(int i = 0; i < 1000; ++ i) {
            std::cout << i << " ";
        }
        std::cout << std::endl;
    }, "ttt");

    t.start();
    // t.join();
}

void EventLoopThreadPool_test() {
    dwt::EventLoop loop;
    dwt::EventLoopThreadPool pool(&loop, "dwt_");
    pool.setThreadNum(2);
    pool.start();

    loop.loop();
}

void Socket_test() {
    
    int fd = ::socket(AF_INET, SOCK_STREAM, 0);

    dwt::Socket skt(fd);

    skt.bindAddress(dwt::InetAddress("127.0.0.1", 8888));

    skt.listen();

    dwt::InetAddress remote;

    skt.accept(&remote);


    std::cout << remote.toIpPort() << std::endl;
}

void Acceptor_test() {
    dwt::EventLoop loop;

    dwt::Acceptor acceptor(&loop, dwt::InetAddress("127.0.0.1", 8888), true);

    acceptor.setNewConnectionCallback([](int fd, const dwt::InetAddress& addr) {
        std::cout << "new connection: " << fd << " " << addr.toIpPort() << std::endl;
    });

    acceptor.listen();
}


/**
 * 回声服务器
 */
void TcpServer_test() {
    dwt::EventLoop loop;
    dwt::InetAddress addr("127.0.0.1", 8080);
    dwt::TcpServer server(&loop, addr, "test");


    server.setConnectionCallback([](const dwt::TcpConnectionPtr& conn) {
        std::cout << "new Connection\n";
        if(!conn->connected()) {
            conn->shutdown();
        }
    });

    // echo
    server.setMessageCallback([](const dwt::TcpConnectionPtr& conn, dwt::Buffer* buf, dwt::Timestamp time) {
        std::string data = buf->retrieveAllAsString();
        std::cout << "[DWT]\t receive:<<<" << data << ">>>" << std::endl;
        conn->send(data);
    });

    server.setThreadNum(4);

    server.start();
    loop.loop();
}

int main() {

    Logger_test();

    // InetAddress_test();

    // Poller_test();

    // EventLoop_test();

    // Thread_test();

    // EventLoopThreadPool_test();

    // Socket_test();

    // Acceptor_test();

    TcpServer_test();
    
    return 0;
}