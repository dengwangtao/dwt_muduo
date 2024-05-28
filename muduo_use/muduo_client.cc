#include <muduo/net/TcpClient.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/base/Logging.h>
#include <muduo/net/Buffer.h>

#include <iostream>
#include <string>
#include <functional>
#include <thread>

using namespace muduo;
using namespace muduo::net;

class EchoClient {
public:
    EchoClient(EventLoop* loop, const InetAddress& serverAddr)
        : client_(loop, serverAddr, "EchoClient") {
        
        client_.setConnectionCallback(
            std::bind(&EchoClient::onConnection, this, std::placeholders::_1));
        client_.setMessageCallback(
            std::bind(&EchoClient::onMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    }

    void connect() {
        client_.connect();
    }

    void send(const std::string& message) {
        if (connection_) {
            connection_->send(message);
        }
    }

private:
    void onConnection(const TcpConnectionPtr& conn) {
        if (conn->connected()) {
            LOG_INFO << "Connected to " << conn->peerAddress().toIpPort();
            connection_ = conn;
        } else {
            LOG_INFO << "Disconnected from " << conn->peerAddress().toIpPort();
            connection_.reset();
        }
    }

    void onMessage(const TcpConnectionPtr& conn, Buffer* buffer, Timestamp time) {
        std::string msg(buffer->retrieveAllAsString());
        std::cout << "Echo from server: " << msg << std::endl;
    }

    TcpClient client_;
    TcpConnectionPtr connection_;
};

void inputThreadFunc(EchoClient& client) {
    std::string line;
    while (std::getline(std::cin, line)) {
        client.send(line + "\n");
    }
}

int main(int argc, char* argv[]) {

    EventLoop loop;
    InetAddress serverAddr("127.0.0.1", 8888);
    EchoClient client(&loop, serverAddr);

    client.connect();

    // 启动一个线程用于处理用户输入
    std::thread inputThread(inputThreadFunc, std::ref(client));

    loop.loop();

    // 等待输入线程结束
    inputThread.join();
    return 0;
}


// g++ muduo_client.cc -o client -lmuduo_net -lmuduo_base -lpthread