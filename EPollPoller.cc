#include "EPollPoller.h"
#include "EventLoop.h"
#include "Logger.h"
#include "Channel.h"

#include <asm-generic/errno-base.h>
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

Timestamp EPollPoller::poll(int timeoutMs, ChannelList *activeChannels) {

    // actually LOG_DEBUG is more resonable
    LOG_INFO("func = %s ==> fd total count: %zu\n", __FUNCTION__, channels_.size());

    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int saveErrno = errno;
    
    Timestamp now(Timestamp::now());

    if (numEvents > 0) {

        LOG_INFO("%d events happened\n", numEvents);
        fillActiveChannel(numEvents, activeChannels); 
        if (numEvents == events_.size()) {
            events_.resize(events_.size() * 2);
        }
    } else if (numEvents == 0) { LOG_INFO("%s Timeout\n", __FUNCTION__);}
    else {

        if (saveErrno != EINTR) {
            errno = saveErrno;
            LOG_ERROR("EPollPoller::poll error: %d", errno);
        }
    }
    return now;
}

// channel update / remove ==> Eventloop updatechannel / removechannel ==> poller updatechannel
void EPollPoller::updateChannel(Channel *channel) {
 
    const int index = channel->index();
    LOG_INFO("func: %s fd = %d events = %d index = %d\n", __FUNCTION__, channel->fd(), channel->events(), index);

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

    LOG_INFO("func: %s fd = %d", __FUNCTION__, fd);
    
    channels_.erase(fd);
    if (index == kAdded) update(EPOLL_CTL_DEL, channel);
    channel->set_index(kNew);
}

// fill active connection
void EPollPoller::fillActiveChannel(int numEvents, ChannelList *activeChannels) const {
    
    for (int i = 0; i < numEvents; i ++) {

        Channel *channel = static_cast<Channel *>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);

    }
}

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
