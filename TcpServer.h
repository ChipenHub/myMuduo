#pragma once

#include "EventLoop.h"
#include "Acceptor.h"
#include "InetAddress.h"
#include "noncopyable.h"
#include "EventLoopThreadPool.h"
#include "Callbacks.h"


#include <memory>
#include <string>
#include <functional>
#include <unordered_map>

// used by server program class
class TcpServer {
public:
    using ThreadInitCallback = std::function<void(EventLoop *)>;

    enum Option {
        kNoReusePort,
        kReusePort,
    };

    TcpServer(EventLoop *loop, const InetAddress & listenAddr, Option option = kNoReusePort);
    ~TcpServer();

    void setThreadInitCallback(const ThreadInitCallback& cb) { ThreadInitCallback_ = cb; }
    void setConnectionCallback(const ConnectionCallback& cb) { ConnectionCallback_ = cb; }
    void setMessageCallback(const MessageCallback& cb) { MessageCallback_ = cb; }
    void setWriteCompleteCallback(const WriteCompleteCallback& cb) { WriteCompleteCallback_ = cb; }



    // set the num of subthreads
    void setThreadNum(int numThreads);
    void start();
private:
    void newConnection(int sockfd, const InetAddress& peerAddr);
    void removeConnection(const TcpConnectionPtr& conn);
    void removeConnectionInLoop(const TcpConnectionPtr &conn);

    using ConnectionMap = std::unordered_map<std::string, TcpConnectionPtr>;

    EventLoop *loop_;  // definite by user
    const std::string ipPort_;
    const std::string name_;

    std::unique_ptr<Acceptor> acceptor_;    // run on mainloop, monitoring new events
    std::shared_ptr<EventLoopThreadPool> threadPool_;   // one loop per thread
    
    ConnectionCallback ConnectionCallback_;
    MessageCallback MessageCallback_;
    WriteCompleteCallback WriteCompleteCallback_;
    ThreadInitCallback ThreadInitCallback_;

    std::atomic_int started_;
    int nextConnId_;
    ConnectionMap Connections_;
};

