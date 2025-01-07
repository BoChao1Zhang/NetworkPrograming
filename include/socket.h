//
// Created by zbc on 25-1-6.
//

#ifndef SOCKET_H
#define SOCKET_H

#include <cstddef>
#include<string>


namespace bizi::socket{
    class Socket
    {
    public:
        Socket();
        ~Socket();

        explicit Socket(int connfd) : m_sockfd(connfd) {}

        int fd() const;
        void clear() { m_sockfd = 0;}
        Socket(const Socket& that) = delete;
        Socket& operator=(const Socket& that) = delete;
        Socket(Socket&& that) noexcept;
        Socket& operator=(Socket&& that) noexcept;

        bool bind(const std::string& ip,int port) const;
        bool listen(int backlog) const;
        bool connect(const std::string& ip,int port) const;
        bool close();
        int accept() const;

        bool setNonBlocking();
        bool setSendBuf(size_t size);
        bool setRecvBuf(size_t size);
        bool setLinger(int active, int seconds);
        bool setKeepAlive();
        bool setReuseAddr();


        size_t send(char *buf, size_t len) const;
        size_t recv(char * buf, size_t len) const;

    private:
        int m_sockfd;
        int m_port = 65536;
        std::string m_ip;
    };

}





#endif //SOCKET_H
