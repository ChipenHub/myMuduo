#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include <cstdio>
#include <memory>


EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop_, const std::string &nameArg) 
    : baseloop_(baseloop_)
    , name_(nameArg)
    , numThreads_(0)
    , next_(0) {}

void EventLoopThreadPool::start(const ThreadInitCallback& cb) {
     started_ = true;
     for (int i = 0; i < numThreads_; ++ i) {
        char buf[name().size() + 32];
        snprintf(buf, sizeof buf, "%s%d", name().c_str(), i);
        EventLoopThread *t = new EventLoopThread(cb, buf);
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startloop());
     }
     // only a baseloop_
     if (numThreads_ == 0 && cb) {
        cb(baseloop_);
     }

}

EventLoop *EventLoopThreadPool::getNextLoop() {
    EventLoop *loop = baseloop_;
    if (!loops_.empty()) {
        loop = loops_[next_ ++];
        if (next_ >= loops_.size()) next_ = 0;
    }
    return loop;
}

std::vector<EventLoop *> EventLoopThreadPool::getAllLoops() {
    if (loops_.empty()) {
        return std::vector<EventLoop *>(1, baseloop_);
    } else { return loops_; }

}
