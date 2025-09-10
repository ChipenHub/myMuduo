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
        
        // handle event when received notion by poller
        void handleEvent(Timestamp receiveTime);

        // set callback function object
        void setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }
        void setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }
        void setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }
        void setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

        void tie(const std::shared_ptr<void> &);

        int fd() const { return fd_; }
        int events() const { return events_; }
        void set_revents(int revt) { revents_ = revt; }

        // set the state of fd echo 
        void enableReading() { events_ |= kReadEvent; update(); }
        void disableReading() { events_ &= ~kReadEvent; update(); }
        
        void enableWriting() { events_ |= kWriteEvent; update(); }
        void disableWriting() { events_ &= ~kWriteEvent; update(); }

        void disableall() {events_ = kNoneEvent; update(); }

        // return current fd state
        bool isNoneEvent() const { return events_ == kNoneEvent; }
        bool isWriting() const { return events_ & kWriteEvent; }
        bool isReading() const { return events_ & kReadEvent; }

        // index state
        int index() { return index_; }
        void set_index(int idx) { index_ = idx; }

        // one loop one thread
        EventLoop *ownerLoop() { return loop_; }
        void remove();

private:

        void update();
        void handleEventWithGuard(Timestamp receiveTime);

        // the which Event was intreasted by fd_
        // the detail of fd_
        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;

        EventLoop *loop_;       // event loop
        const int fd_;          // fd, monitored by Poller
        int events_;            // log up event
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
