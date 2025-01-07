#pragma once

#include<sys/select.h>

namespace bizi::socket {
    class Selector {
    public:
        Selector();
        ~Selector() = default;

        void set(int fd);
        void del(int fd);
        int select(long milliseconds);
        int maxFd() const;
        bool isSet(int fd);

    
    private:
        fd_set m_fds;
        fd_set m_readFds;
        int m_maxFd;
    };

}