#include "Poller.h"
#include <stdlib.h>

Poller *Poller::newDefaultPoller(EventLoop *loop) {
    
    if (::getenv("MODUO_USE_POLL")) { return nullptr; } // generate Poll instance
    else return nullptr;                                // generate epoll instance 

}

