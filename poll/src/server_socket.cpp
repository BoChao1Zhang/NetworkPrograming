#include "server_socket.h"


using namespace bizi::socket;

ServerSocket::ServerSocket(std::string const &ip, int port) {
    // setNonBlocking();
    setSendBuf(BUF_SIZE);
    setRecvBuf(BUF_SIZE);
    setReuseAddr();
    setKeepAlive();
    bind(ip, port);
    listen(LOG_SIZE);
}

