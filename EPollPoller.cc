#include "EPollPoller.h"
#include "EventLoop.h"
#include "Logger.h"
#include "Channel.h"

#include <unistd.h>
#include <cstring>
#include <sys/epoll.h>


class Eventloop;

// channel didn't add into polelr
const int kNew = -1; // channel's member: index_ = -1
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
 
    const int index = channel->index();
    LOG_INFO("fd = %d events = %d index = %d\n", channel->fd(), channel->events(), index);

    if (index == kNew || index == kDeleted) {
        if (index == kNew) {
            int fd = channel->fd();
            channels_[fd] = channel;
        }

        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        
        int fd = channel->fd();
        if (channel->isNoneEvent()) {
            
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);

        } else {

            update(EPOLL_CTL_MOD, channel);

        }

    }


}

void EPollPoller::removeChannel(Channel *channel) {

    int fd = channel->fd();
    int index = channel->index();
    
    channels_.erase(fd);

    if (index == kAdded) update(EPOLL_CTL_DEL, channel);
    channel->set_index(kNew);

}

// fill active connection
void EPollPoller::fillActiveChannel(int numEvents, ChannelList *activeChannels) const {}

void EPollPoller::update(int operation, Channel *channel) {
   
    epoll_event event;
    memset(&event, 0, sizeof event);

    int fd = channel->fd();

    event.events = channel->events();
    event.data.fd = fd;
    event.data.ptr = channel;

    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
    
        if (operation == EPOLL_CTL_DEL) LOG_ERROR("epoll_ctl_del error: %d", errno);
        else LOG_FATAL("epoll_ctl_del error: %d", errno);
    }
}
