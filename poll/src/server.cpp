
#include <array>
#include <cstdio>
#include<singleton.h>
#include<server_socket.h>

#include "logger.h"
using namespace bizi::socket;
using namespace bizi::utility;




int main(){
    Singleton<Logger>::instance()->open("../server.log");
    //创建一个socket
    // Socket server;
    
    // server.bind("127.0.0.1",8080);

    // server.listen(1024);

    ServerSocket server("127.0.0.1",8080);


    while(true){
        int connfd = server.accept();

        Socket client = Socket(connfd);

        std::array<char, 1024>buf{};

        size_t len = client.recv(buf.data(),buf.size());

        std::string msg{buf.data(),len};
        printf("%s\n",msg.c_str());

        client.send(buf.data(), len);
    }
    
}