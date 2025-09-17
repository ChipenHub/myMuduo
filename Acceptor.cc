#include "Acceptor.h"
#include "Logger.h"
#include "InetAddress.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <functional>
#include <unistd.h>

static int createNonblocking() {
    int sockfd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
    if (sockfd < 0) LOG_FATAL("%s:%s:%d listen socket create error: %d", __FILE__, __FUNCTION__, __LINE__, errno);
    return sockfd;
}

Acceptor::Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport) 
    : loop_(loop)
    , acceptSocket_(createNonblocking())
    , acceptChannel_(loop, acceptSocket_.fd())
    , listening_(false) {
    acceptSocket_.bindAddress(listenAddr);
    // TcpServer::start(), Acceptor.listen() operate callback
    acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead, this));
}
Acceptor::~Acceptor() {}
void listen();
void Acceptor::handleRead() {
    InetAddress peerAddr;
    int connfd = acceptSocket_.accept(&peerAddr);
    if (connfd >= 0) {
        if (NewConnectionCallback_) {
            NewConnectionCallback_(connfd, peerAddr);
        } else {
            ::close(connfd);
        }
    } else {
        LOG_ERROR("%s:%s:%d accept socket create error: %d", __FILE__, __FUNCTION__, __LINE__, errno);
    }
}

