#include "server_socket.h"
#include <array>
#include <cstring>
#include <logger.h>
#include <select_handle.h>

using namespace bizi::socket;
using namespace bizi::utility;

void SelectHandle::listen(std::string const &ip, int port) {
    m_server = new ServerSocket(ip, port);
}

void SelectHandle::attach(Socket *socket) {
    int fd = socket->fd();

    if (m_conns.contains(fd)) {
        return;
    }

    m_selector.set(fd);
    m_conns[fd] = socket;
}

void SelectHandle::detach(Socket *socket) {
    int fd = socket->fd();
    if (m_selector.isSet(fd)) {
        m_selector.del(fd);
    }
}

void SelectHandle::remove(Socket *socket) {
    int fd = socket->fd();

    detach(socket);
    m_conns.erase(fd);
    socket->close();
    delete socket;
}

int SelectHandle::handle(long timeout) {
    attach(m_server);

    while (true) {
        int nready = m_selector.select(timeout);

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

        for (int fd = 0; fd < m_selector.maxFd() + 1; fd++) {
            if (m_selector.isSet(fd)) {
                if (fd == m_server->fd()) {
                    int connfd = m_server->accept();

                    if (connfd < 0) {
                        continue;
                    }

                    Socket *socket = new Socket(connfd);
                    attach(socket);
                    log_debug("accept success: connfd = %d", connfd);
                } else {
                    auto client = m_conns[fd];
                    detach(client);
                    std::array<char, 1024> buf{};

                    size_t len = client->recv(buf.data(), buf.size());
                    log_debug("recv len:%d", len);
                    if (len == 0) {
                        log_debug("connection close: connfd = %d", fd);
                        client->clear();
                        remove(client); // 服务端不主动关闭连接
                    } else if (len > 0) {
                        std::string msg(buf.begin(), buf.begin() + len);
                        log_debug("recv data: %s", msg.data());
                        client->send(msg.data(), msg.size());
                    }

                    attach(client);
                }
            }
        }
    }

    return 0;
}
