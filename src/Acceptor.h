#pragma once

#include "noncopyable.h"
#include "Socket.h"
#include "Channel.h"

#include <functional>

namespace dwt {


class EventLoop;
class InetAddress;



class Acceptor : noncopyable {

public:
    using NewConnectionCallback = std::function<void(int fd, const InetAddress&)>;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reusePort);

    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb) {
        m_newConnectionCallback = cb;
    }

    bool listening() const { return m_listening; }

    void listen();

private:

    void handleRead();

    EventLoop* m_loop;
    Socket m_acceptSocket;
    Channel m_acceptChannel;

    bool m_listening;

    NewConnectionCallback m_newConnectionCallback;
};


}