#include "Channel.h"
#include "EventLoop.h"
#include "Logger.h"
#include "Poller.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <fcntl.h>
#include <mutex>

// ensure one thread one loop, ps: __thread means thread local
__thread EventLoop *t_loopInThisThread = nullptr;

// define default TimeoutMs fo Poller
const int KPollerTimeMs = 10000;      // 10s

int createEventfd() {
    
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0) {
        LOG_FATAL("eventfd error: %d\n", errno);
    }
    return evtfd;
}

EventLoop::EventLoop()
    : looping_(false)
    , quit_(false)
    , callingPendingFunctors_(false)
    , threadId_(CurrentThread::tid())
    , poller_(Poller::newDefaultPoller(this))
    , wakeupfd_(createEventfd())
    , wakeupChannel_(new Channel(this, wakeupfd_)) {    
        LOG_DEBUG("EventLoop created %p in thread %d\n", this, threadId_);

        if (t_loopInThisThread != nullptr) LOG_FATAL("another eventloop %p exists in this loop %d", this, threadId_);
        else t_loopInThisThread = this;

        wakeupChannel_->setReadCallback(std::bind(&EventLoop::handleRead, this));
        wakeupChannel_->enableReading();

    }

EventLoop::~EventLoop() {

    wakeupChannel_->disableall();
    wakeupChannel_->remove();
    ::close(wakeupfd_);
    t_loopInThisThread = nullptr;

}

void EventLoop::loop() {

    looping_ = true;
    quit_ = false;

    LOG_INFO("Eventloop %p start looping \n", this);

    while (!quit_) {
    
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(KPollerTimeMs, &activeChannels_);
        for (Channel *channel : activeChannels_) {
            // poller can observe which channels' events happened, and tell eventloopo
            channel->handleEvent(pollReturnTime_);
        }
        // execute callback operation that needed by current Eventloop
        doPendingFunctors();

    }
    
    LOG_INFO("Eventloop %p stop looping.\n", this);
    looping_ = false;
}

void EventLoop::quit() {
    quit_ = true;

    if (!isInLoopThread()) wakeup();
}


void EventLoop::runInloop(Functor cb) {

    if (isInLoopThread()) {

       cb(); 

    } else {
    
        queueInloop(cb);

    }

}

void EventLoop::queueInloop(Functor cb) {
    
    {
        std::unique_lock<std::mutex> lock(mutex_);
        pendingFunctor_.emplace_back(cb);
    }
    
    // "|| callingPendingFunctors_" means current loop is opreating callback, while loop got a new pending
    if (!isInLoopThread() || callingPendingFunctors_) { wakeup(); }

}

void EventLoop::wakeup() {

    uint64_t one = 1;
    ssize_t n = write(wakeupfd_, &one, sizeof one);
    if (n != sizeof one) { LOG_ERROR("eventloop writes %lu bytes instead of 8\n", n); }

}

void EventLoop::handleRead() {
    uint64_t one = 1;
    ssize_t n = read(wakeupfd_, &one, sizeof one);
}


void EventLoop::updateChannel(Channel *channel) {

    poller_->updateChannel(channel);

}
void EventLoop::removeChannel(Channel *channel) {

    poller_->updateChannel(channel);

}
bool EventLoop::hasChannel(Channel *channel) {

    return poller_->hasChannel(channel);

}

void EventLoop::doPendingFunctors() {

    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        std::unique_lock<std::mutex> lock(mutex_);
        functors.swap(pendingFunctor_);
    }

    for (const Functor& Functor : functors) {
       Functor();       // call pending operation needed by current loop 
    }

    callingPendingFunctors_ = false;

}

