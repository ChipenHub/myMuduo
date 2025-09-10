#pragma once

#include <vector>
#include <sys/epoll.h>

#include "Poller.h"
#include "Timestamp.h"


// epoll : epoll_create -> epoll_ctl(add, modify, del) -> epoll_wait

class EPollPoller : public Poller {
public:
    EPollPoller(EventLoop *Loop);
    ~EPollPoller() override;

    // override base_class's(poller) obstract way
    Timestamp poll(int timeoutMs, ChannelList *active) override; 

    void updateChannel(Channel *channel) override;
    void removeChannel(Channel *channel) override;

private:

    static const int kInitEventListSize = 16;

    // fill active connection
    void fillActiveChannel(int numEvents, ChannelList *activeChannels) const;
    // update channel access
    void update(int operation, Channel *channel);

    using EventList = std::vector<epoll_event>;

    int epollfd_;
    EventList events_; 


};
