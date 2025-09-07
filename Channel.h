#pragma once

#include "noncopyable.h"
#include <functional>
#include <memory>

class EventLoop;
class Timestamp;

// as a channel, encode the sockfd and event that was intreasted events like epollin, epollout
class Channel : noncopyable {
    public:
        using EventCallback = std::function<void()>;
        using ReadEventCallback = std::function<void(Timestamp)>;

        Channel(EventLoop *EventLoop, int fd);
        ~Channel();

        void handleEvent(Timestamp receiveTime);
    private:
        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;

        EventLoop *loop_;       // event loop
        const int fd_;          // fd, monitored by Poller
        int evnets_;            // log up event
        int revents_;           // returned by Poller
        int index_;

        std::weak_ptr<void> tie_;
        bool tied_;

        // channel can call the challback because it knows the revents
        ReadEventCallback readCallback_;
        EventCallback writeCallback_;
        EventCallback closeCallback_;
        EventCallback errorCallback_;

};
