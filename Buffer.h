#pragma once

#include <vector>
#include <string>
#include <algorithm>

class Buffer {
public:
    static const std::size_t kCheapPrepend = 8;
    static const std::size_t kInitalSize = 1024;

    explicit Buffer(std::size_t kinitalSize = kInitalSize)
        : buffer_(kCheapPrepend + kInitalSize)
        , readerIndex_(kCheapPrepend)
        , writerIndex_(kCheapPrepend) {} 

    std::size_t readableBytes() const { return writerIndex_ - readerIndex_; }
    std::size_t writableBytes() const { return buffer_.size() - writerIndex_; }

    std::size_t prependableBytes() const { return readerIndex_; }

    std::string retriveAllAsString() { return retriveAsString(readableBytes()); }
    std::string retriveAsString(std::size_t len) {
        std::string result(peek(), len);
        retrive(len);
        return result;
    }
    
    void ensureWriteableBytes(std::size_t len) {
        if (writableBytes() < len) {
            makeSpace(len);
        }
    }

    void makeSpace(std::size_t len) {
        if (writableBytes() + prependableBytes() < len + kCheapPrepend) {
            buffer_.resize(readerIndex_ + len);
        } else {
            std::copy(begin() + readerIndex_,
                    begin() + writerIndex_,
                    begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = kCheapPrepend + readableBytes();
        }
    }

    // add [data, data + len) into buffer
    void append(const char* data, std::size_t len) {
        ensureWriteableBytes(len);
        std::copy(data, data + len, beginWrite()); 
        writerIndex_ += len;
    }

    char* beginWrite() { return begin() + writerIndex_; }
    const char* beginWrite() const { return begin() + writerIndex_; }
    
    ssize_t readFd(int fd, int* saveErrno);
private:
    char* begin() { return &*buffer_.begin();}  // the first element's address
    const char* begin() const { return &*buffer_.begin();} 

    const char* peek() const { return begin() + readerIndex_; }

    void retrive(size_t len) {
        if (len < readableBytes()) {
            readerIndex_ += len;        // application only read a part of readablebuffer
        } else {
            retriveAll(); 
        }
    }
    void retriveAll() {
        readerIndex_ = writerIndex_ = kCheapPrepend;
    }

    std::vector<char> buffer_;
    std::size_t readerIndex_;
    std::size_t writerIndex_;
};
