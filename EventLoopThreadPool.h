#pragma once
#include "noncopyable.h"
#include <functional>
#include <memory>
#include <string>
#include <vector>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : noncopyable {
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;

    EventLoopThreadPool(EventLoop *baseloop_, const std::string &nameArg);
    ~EventLoopThreadPool();

    void setThreadNum(int numThreads) { numThreads_ = numThreads; }
    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    // if work on multi-thread, baseloop arrange channels to loops by polling
    EventLoop *getNextLoop();
    
    std::vector<EventLoop *> getAllLoops();

    bool started() { return started_; }
    const std::string name() { return name_; }
private:
    EventLoop *baseloop_;    // eventloop
    std::string name_;
    bool started_;
    int numThreads_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop *> loops_;


};
