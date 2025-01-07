#pragma once

#include<sys/select.h>

namespace bizi::socket {
    class Selector {
    public:
        Selector();
        ~Selector();

        void set(int fd);
        void del(int fd);
        int select(int milliseconds);
        int maxFd() const;
        bool isSet(int fd);

    
    private:
        fd_set m_fds;
        fd_set m_readFds;
        int m_maxFd;
    };

}