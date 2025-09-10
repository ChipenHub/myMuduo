#pragma once

#include <vector>
#include <unordered_map>

#include "EventLoop.h"
#include "Timestamp.h"
#include "noncopyable.h"

class Channel;
class EventLoop;

class Poller : noncopyable {
    
public:
    using ChannelList = std::vector<Channel *>;

    Poller(EventLoop *loop);

    virtual ~Poller() = default;

    // keep unified interface
    virtual Timestamp poll(int tiemoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(Channel *channel) = 0;
    virtual void removeChannel(Channel *channel) = 0;

    // check if channel is in current poller
    bool hasChannel(Channel *channel) const;

    static Poller *newDefaultPoller(EventLoop *loop);
    // EventLoop can get a default io multiplexing interface
protected:
    // key -> fd_; Channel * -> fd::channels
    using ChannelMap = std::unordered_map<int, Channel *>;
    ChannelMap channels_;

private:
    EventLoop *ownerLoop_;

};
