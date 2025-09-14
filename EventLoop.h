#pragma once

#include "noncopyable.h"
#include "Timestamp.h"
#include "CurrentThread.h"

#include <memory>
#include <functional>
#include <vector>
#include <atomic>
#include <unistd.h>
#include <mutex>


class Channel;
class Poller;
// a time loop class, include two part: channel and poller
class EventLoop {
public:
    using Functor = std::function<void()>;

    EventLoop();
    ~EventLoop();

    // start event loop
    void loop();
    // quit event loop
    void quit();

    Timestamp pollReturnTime() const { return pollReturnTime_; }

    // call cb in current loop
    void runInloop(Functor cb);
    // put cb in queue, wake up current loop and call it
    void queueInloop(Functor cb);

    // wake up the thread where loop at
    void wakeup();

    void updateChannel(Channel *channel);
    void removeChannel(Channel *channel);
    bool hasChannel(Channel *channel);

    // check if Eventloop is in its thread
    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

private:

    void handleRead();  // wakeup
    void doPendingFunctors();   // run callback

    using ChannelList = std::vector<Channel *>;

    std::atomic_bool looping_;  // CAS
    std::atomic_bool quit_;     // record exit loop

    const pid_t threadId_;  // record current thread's id 
    Timestamp pollReturnTime_;  // time point of channels returned by poll
    std::unique_ptr<Poller> poller_;

    int wakeupfd_;
    std::unique_ptr<Channel> wakeupChannel_;

    ChannelList activeChannels_;
    
    std::atomic_bool callingPendingFunctors_;   // record current loop if need to handle recall
    std::vector<Functor> pendingFunctor_;       // save all the recalls needed call for loop
    std::mutex mutex_;                          // guard vector to pthread safe
};
