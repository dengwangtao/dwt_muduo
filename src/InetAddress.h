#pragma once

#include <string>
#include <arpa/inet.h>

namespace dwt {


class InetAddress {

public:
    explicit InetAddress(const std::string& IP, uint16_t port);
    explicit InetAddress(const struct sockaddr_in& addr);

    std::string toIp() const;
    uint16_t toPort() const;
    std::string toIpPort() const;

    const struct sockaddr_in* getSockAddr() const;

private:
    struct sockaddr_in m_addr;

};

}