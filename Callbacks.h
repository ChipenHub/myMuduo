#pragma once

#include "Timestamp.h"
#include <functional>
#include <memory>

class Buffer;
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(TcpConnectionPtr&)>;
using CloseCallback = std::function<void(TcpConnectionPtr&)>;
using WriteCompleteCallback = std::function<void(TcpConnectionPtr&)>;

using MessageCallback = std::function<void (const TcpConnectionPtr&, Buffer *, Timestamp)>;
