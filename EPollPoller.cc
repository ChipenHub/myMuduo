#include "EPollPoller.h"
#include "EventLoop.h"
#include "Logger.h"

#include <unistd.h>
#include <sys/epoll.h>

class Eventloop;

// channel didn't add into polelr
const int kNew = -1; // channel->index_ = -1
// channel has added into poller
const int kAdded = 1;
// channel was deleted from poller
const int kDeleted = 2;

// epoll_create1 is able to close the old fd interface
EPollPoller::EPollPoller(EventLoop *loop) : Poller(loop), epollfd_(::epoll_create1(EPOLL_CLOEXEC)), events_(kInitEventListSize) {
    if (epollfd_ < 0) {
        LOG_FATAL("epoll_create error: %d\n", errno);
    }

}

EPollPoller::~EPollPoller() { ::close(epollfd_); }

Timestamp EPollPoller::poll(int timeoutMs, ChannelList *activeChannels) {}

// channel update / remove ==> Eventloop updatechannel / removechannel ==> poller updatechannel
void EPollPoller::updateChannel(Channel *channel) {
 


}

void EPollPoller::removeChannel(Channel *channel) {

}

