#include <algorithm>
#include <array>
#include <client_socket.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <logger.h>
#include <server_socket.h>
#include <singleton.h>
#include <sys/select.h>

using namespace bizi::socket;
using namespace bizi::utility;

int main() {
    Singleton<Logger>::instance()->open("../select.log");
    ServerSocket server("127.0.0.1", 8080);

    fd_set fds;
    FD_ZERO(&fds);

    FD_SET(server.fd(), &fds);
    int max_fd = server.fd();

    while (true) {
        fd_set readfds = fds;

        int nready = select(max_fd + 1, &readfds, nullptr, nullptr, nullptr);

        if (nready < 0) {
            log_error("select error: errno=%d, errmsg=%s", errno,
                      strerror(errno));
            break;
        }

        if (nready == 0) {
            log_debug("select timeout");
            continue;
        }

        log_debug("select ok: nready = %d", nready);

        for (int fd = 0; fd < max_fd + 1; fd++) {
            if (FD_ISSET(fd, &readfds)) {
                if (fd == server.fd()) {
                    int connfd = server.accept();

                    if (connfd < 0) {
                        continue;
                    }

                    log_debug("accept success: connfd = %d",connfd);

                    FD_SET(connfd, &fds);

                    max_fd = std::max(max_fd, connfd);
                } else {
                    Socket client(fd);
                    std::array<char, 1024> buf{};

                    int len = client.recv(buf.data(), buf.size());
                    log_debug("recv len:%d",len);
                    if (len == 0) {
                        log_debug("connection close: connfd = %d", fd);
                        FD_CLR(fd, &fds);
                        client.clear(); // 服务端不主动关闭连接
                    } else if (len > 0) {
                            std::string msg(buf.begin(), buf.begin() + len);
                            log_debug("recv data: %s", msg.data());
                            client.send(msg.data(), msg.size());
                    }

                    client.clear();
                }
            }
        }
    }
}
