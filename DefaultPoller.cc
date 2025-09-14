#include "Poller.h"
#include "EPollPoller.h"
#include <stdlib.h>

Poller *Poller::newDefaultPoller(EventLoop *loop) {
    
    if (::getenv("MODUO_USE_POLL")) { return nullptr; } // generate Poll instance
    else return new EPollPoller(loop);                                // generate epoll instance 

}

