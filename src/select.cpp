#include <array>
#include <client_socket.h>
#include <cstdio>
#include <cstring>
#include <logger.h>
#include <server_socket.h>
#include <singleton.h>
#include <sys/select.h>
#include <selector.h>
#include <select_handle.h>

using namespace bizi::socket;
using namespace bizi::utility;

int main() {
    Singleton<Logger>::instance()->open("../select.log");

    auto selectHandle = Singleton<SelectHandle>::instance();
    selectHandle->listen("127.0.0.1", 8080);

    selectHandle->handle(1000);
    
}
