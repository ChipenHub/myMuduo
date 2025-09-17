#include "Socket.h"
#include "InetAddress.h"
#include "Logger.h"

#include <netinet/in.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <unistd.h>

Socket::~Socket() { ::close(sockfd_); }

void Socket::bindAddress(const InetAddress &localaddr) {
  if (bind(sockfd_, (const struct sockaddr *)localaddr.getSockAddr(), sizeof(struct sockaddr_in)) != 0) {
      LOG_FATAL("bind sockfd:%d fail\n", sockfd_);
  }
}

void Socket::listen() {
    if (0 != ::listen(sockfd_, 1024)) {
      LOG_FATAL("Listen sockfd:%d fail\n", sockfd_);
    }

}
int Socket::accept(InetAddress *peeraddr) {
    sockaddr_in addr;
    bzero(&addr, sizeof addr);
    socklen_t len;
    int connfd = ::accept(sockfd_, (sockaddr *)&addr, &len);
    if (connfd >= 0) {
        peeraddr->setSockAddr(addr);
    }
    return connfd;
}
void Socket::shutdownWrite() {
   if (::shutdown(sockfd_, SHUT_WR) < 0) {
       LOG_ERROR("shutdown error");
   }
}
void Socket::setTcpNoDely(bool on) {
    int optval = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof optval);
}
