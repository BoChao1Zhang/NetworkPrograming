#pragma once

#include <cstddef>
#include <string>
#include <socket.h>
namespace bizi::socket {

    class ServerSocket : public Socket {
    public:
        ServerSocket() = delete;
        ServerSocket(std::string const &ip, int port);
        ~ServerSocket() = default;
    
    private:
        static size_t const BUF_SIZE = 10 * 1024;
        static size_t const LOG_SIZE = 1024;
    };

}