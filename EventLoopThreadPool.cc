#include "EventLoopThreadPool.h"
#include <cstdio>


EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseloop_, const std::string &nameArg) 
    : baseloop_(baseloop_)
    , name_(nameArg)
    , numThreads_(0)
    , next_(0) {}
void EventLoopThreadPool::start(const ThreadInitCallback& cb) {
     started_ = true;
     for (int i = 0; i < numThreads_; ++ i) {
        char buf[name.size() + 32];
        snprintf(buf, sizeof buf, "%s%d", nameArg.c_str(), i);
     }
     // only a baseloop_
     if (numThreads_ == 0 && cb) {
        cb(baseloop_);
     }

}
EventLoop *EventLoopThreadPool::getNextLoop() {}
std::vector<EventLoop *> EventLoopThreadPool::getAllLoops() {}
