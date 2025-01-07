#pragma once

#include <socket.h>
#include <string>

namespace bizi::socket {

class ClientSocket : public Socket {
public:
    ClientSocket() = delete;
    ClientSocket(std::string const &ip, int port);
    ~ClientSocket() = default;

private:
};
} // namespace bizi::socket
