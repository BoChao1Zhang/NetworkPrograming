#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <cerrno>
#include <cstddef>
#include <cstring>
#include <fcntl.h>
#include <logger.h>
#include <netinet/in.h>
#include <singleton.h>
#include <socket.h>
#include <sys/socket.h>
#include <fcntl.h>

using namespace bizi::socket;
using namespace bizi::utility;

Socket::Socket(Socket &&that) noexcept
    : m_sockfd(that.m_sockfd),
      m_port(that.m_port),
      m_ip(std::move(that.m_ip)) {
    log_debug("Socket move constructor called");
    that.m_sockfd = -1;
    that.m_port = -1;
    that.m_ip.clear();
}

Socket &Socket::operator=(Socket &&that) noexcept {
    log_debug("Socket move assignment operator called");
    if (&that == this) {
        return *this;
    }

    m_sockfd = that.m_sockfd;
    m_ip = std::move(that.m_ip);
    m_port = that.m_port;

    that.m_sockfd = -1;
    that.m_port = -1;
    that.m_ip.clear();

    return *this;
}

bool Socket::bind(std::string const &ip, int port) const {
    log_debug("bind called with ip=%s, port=%d", ip.c_str(), port);
    sockaddr_in sockaddr = {};

    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    sockaddr.sin_addr.s_addr =
        ip.empty() ? htonl(INADDR_ANY) : inet_addr(ip.c_str());
    if (::bind(m_sockfd, reinterpret_cast<const struct sockaddr *>(&sockaddr),
               sizeof(sockaddr)) < 0) {
        log_error("bind error: errno=%d errmsg=%s", errno, strerror(errno));
        return false;
    }

    return true;
}

bool Socket::listen(int backlog) const {
    log_debug("listen called with backlog=%d", backlog);
    if (::listen(m_sockfd, backlog) < 0) {
        log_error("listen error: errno=%d errmsg=%s", errno, strerror(errno));
        return false;
    }

    return true;
}

bool Socket::connect(std::string const &ip, int port) const {
    log_debug("connect called with ip=%s, port=%d", ip.c_str(), port);
    sockaddr_in sockaddr = {};
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_port = htons(port);
    sockaddr.sin_addr.s_addr = inet_addr(ip.c_str());

    if (::connect(m_sockfd,
                  reinterpret_cast<const struct sockaddr *>(&sockaddr),
                  sizeof(sockaddr)) < 0) {
        log_error("connect failed error_code=%d, error_msg=%s", errno,
                  strerror(errno));
        return false;
    }

    log_debug("connect success");
    return true;
}

bool Socket::close() {
    if (m_sockfd > 0 && ::close(m_sockfd) < 0) {
        log_error("close failed: ec=%d, error_msg=%s", errno, strerror(errno));
        return false;
    }

    log_debug("close success: fd = %d",m_sockfd);
    return true;
}

int Socket::accept() const {
    int connfd = ::accept(
        m_sockfd, nullptr, nullptr);
    if (connfd < 0) {
        log_error("accept failed: ec=%d, error_msg=%s", errno, strerror(errno));
        return -1;
    }
    return connfd;
}

size_t Socket::send(char *buf, size_t len) const {
    ssize_t sent = ::send(m_sockfd, buf, len, 0);
    if (sent < 0) {
        log_error("send failed: ec=%d, error_msg=%s", errno, strerror(errno));
    }
    return sent;
}

size_t Socket::recv(char *buf, size_t len) const {
    ssize_t received = ::recv(m_sockfd, buf, len, 0);
    if (received < 0) {
        log_error("recv failed: ec=%d, error_msg=%s", errno, strerror(errno));
    }
    return received;
}

Socket::Socket()
    : m_sockfd(::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)),
      m_port(0) {
    if (m_sockfd < 0) {
        log_error("create socket error: errno=%d errmsg=%s", errno,
                  strerror(errno));
    } else {
        log_debug("create socket success");
    }
}

Socket::~Socket() {
    if (m_sockfd >= 0) {
        close();
    }
}

bool Socket::setNonBlocking() {
    int flag = fcntl(m_sockfd, F_GETFL, 0);

    if (flag < 0) {
        log_error("set non blocking failed: errno= %d,errormsg = %s", errno,
                  strerror(errno));
        return false;
    }

    flag |= O_NONBLOCK;

    if (fcntl(m_sockfd, F_SETFL, flag) < 0) {
        log_error("set non blocking failed: errno= %d,errormsg = %s", errno,
                  strerror(errno));
        return false;
    }

    return true;
}

bool Socket::setSendBuf(size_t size) {
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size)) < 0) {
        log_error("set send buf error: errno=%d,errnoMsg = %s", errno,
                  strerror(errno));
        return false;
    }

    return true;
}

bool Socket::setRecvBuf(size_t size) {
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) < 0) {
        log_error("set recv buf error: errno=%d,errnoMsg = %s", errno,
                  strerror(errno));
        return false;
    }

    return true;
}

/**
 * @brief Sets the linger option on the socket.
 *
 * The linger option controls the action taken when unsent data is queued on a socket
 * and a close() is performed. If active is non-zero, the linger option is enabled, 
 * and the socket will attempt to send any unsent data for the specified number of seconds.
 * If active is zero, the linger option is disabled, and the socket will close immediately.
 *
 * @param active Non-zero to enable linger option, zero to disable.
 * @param seconds Number of seconds to linger if the option is enabled.
 * @return true if the linger option was successfully set, false otherwise.
 */
bool Socket::setLinger(int active, int seconds) {
    struct linger ling = {};
    ling.l_onoff = active;
    ling.l_linger = seconds;

    if (setsockopt(m_sockfd, SOL_SOCKET, SO_LINGER, &ling, sizeof(ling)) < 0) {
        log_error("set linger error: errno=%d,errnoMsg = %s", errno,
                  strerror(errno));
        return false;
    }

    return true;
}

/**
 * @brief 设置TCP keepalive选项
 * 
 * TCP keepalive 用于检测连接是否存活。如果在指定时间内没有数据传输,
 * 则发送探测包来确认连接状态。这可以帮助检测到"半开"连接或对端意外崩溃的情况。
 * 
 * @return true 设置成功
 * @return false 设置失败
 */
bool Socket::setKeepAlive() {
    int keep_alive = 1; // 启用 keepalive
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_KEEPALIVE, &keep_alive, 
                   sizeof(keep_alive)) < 0) {
        log_error("set keep alive error: errno=%d,errnoMsg = %s", errno,
                  strerror(errno));
        return false;
    }
    return true;
}

/**
 * @brief 设置地址重用选项
 * 
 * SO_REUSEADDR 允许在同一端口上启动多个监听socket，适用于以下情况:
 * 1. 服务器重启时立即绑定到相同端口
 * 2. 允许同一机器上的多个socket绑定到相同端口(TCP监听不同地址)
 * 3. 允许完全相同的地址和端口对(仅在UDP协议中)
 *
 * @return true 设置成功
 * @return false 设置失败
 */
bool Socket::setReuseAddr() {
    int reuse = 1; // 启用地址重用
    if (setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, 
                   sizeof(reuse)) < 0) {
        log_error("set reuse addr error: errno=%d,errnoMsg = %s", errno,
                  strerror(errno));
        return false;
    }
    return true;
}

int Socket::fd() const {
    return m_sockfd;
}

// createA test
