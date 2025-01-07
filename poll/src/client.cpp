#include<client_socket.h>
#include<iostream>
#include<singleton.h>
#include "logger.h"

using namespace bizi::socket;
using namespace bizi::utility;

int main() {
    Singleton<Logger>::instance()->open("../client.log");
    // Socket client{};

    // client.connect("127.0.0.1", 8080);

    ClientSocket client("127.0.0.1", 8080);


    char buf[1024];

    std::string message = "Hello, Server!";
    client.send(const_cast<char*>(message.c_str()), message.size());

    size_t len = client.recv(buf, sizeof(buf));
    std::string response{buf, len};
    std::cout << "Received from server: " << response << std::endl;


    return 0;


}
