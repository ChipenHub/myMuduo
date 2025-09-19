#include "TcpServer.h" 
#include "InetAddress.h"
#include "Logger.h"

EventLoop* CheckLoopNotNull(EventLoop* loop) {
    if (loop == nullptr) LOG_FATAL("%s:%s:%d mainLoop is nullptr!\n", __FILE__, __FUNCTION__, __LINE__); 
    return loop;
}

TcpServer::TcpServer(EventLoop* loop, const InetAddress& listenAddr, const std::string nameArg, Option option)
    : loop_(CheckLoopNotNull(loop))
    , ipPort_(listenAddr.toIpPort())
    , name_(nameArg)
    , acceptor_(new Acceptor(loop, listenAddr, option))
    , threadPool_(new EventLoopThreadPool(loop, name_))
    , connectionCallback_()
    , messageCallback_()
    , nextConnId_(1) {
// when a new user connected, newConnection wiil called
    acceptor_->setNewConnectionCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}
void TcpServer::setThreadNum(int numThread) {
    threadPool_->setThreadNum(numThread);
}
void TcpServer::start() {
    if (started_++ == 0) {
        threadPool_->start(threadInitCallback_);        // start loop threadPool
        loop_->runInloop(std::bind(&Acceptor::listen, acceptor_.get()));
    }

}
void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr) {} 
