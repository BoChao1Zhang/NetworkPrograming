#include <client_socket.h>

using namespace bizi::socket;

ClientSocket::ClientSocket(std::string const &ip, int port) {
    connect(ip, port);
}
