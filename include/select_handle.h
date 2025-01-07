#pragma once

#include "selector.h"
#include "singleton.h"
#include <map>
#include <socket.h>

namespace bizi::socket {
using namespace utility;

class SelectHandle {
    SINGLETON(SelectHandle);
    public:
        
        void listen(const std::string& ip, int port);
        void attach(Socket* socket);
        void detach(Socket* socket);
        void remove(Socket *socket);
        int handle(long timeout);

        

    
    private:
        Socket *m_server = nullptr;
        Selector m_selector;
        std::map<int, Socket *> m_conns;
    };

}