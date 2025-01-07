#include <bits/types/struct_timeval.h>
#include <selector.h>
#include <sys/select.h>
#include <algorithm>

using namespace bizi::socket;

Selector::Selector() {
    FD_ZERO(&m_fds);
    FD_ZERO(&m_readFds);
    m_maxFd = 0;
}

void Selector::set(int fd) {
    FD_SET(fd, &m_fds);
    m_maxFd = std::max(fd,m_maxFd);
}

void Selector::del(int fd) {
    FD_CLR(fd, &m_fds);
}

int Selector::select(long milliseconds) {
    
    timeval tv{};
    tv.tv_sec = milliseconds / 1000;
    tv.tv_usec = (milliseconds % 1000) * 1000;

    m_readFds = m_fds;
    //这里一定是m_maxFd + 1,下标从0开始,数量要加1
    return ::select(m_maxFd + 1, &m_readFds, nullptr, nullptr, &tv);
}

int Selector::maxFd() const {
    return m_maxFd;
}

bool Selector::isSet(int fd) {
    //select 会将读信息写入到m_readFds中.
    return FD_ISSET(fd, &m_readFds);
}

