#include "EPollPoller.h"
#include "EventLoop.h"
class Eventloop;

EPollPoller::EPollPoller(EventLoop *loop) : Poller(loop) {}
