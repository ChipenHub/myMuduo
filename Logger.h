#pragma once

#include <string>
#include <iostream>

#include "noncopyable.h"

// LOG_INFO("%s %d", arg1, arg2)
#define LOG_INFO(LogmsgFormat, ...) \
    do { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(INFO); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, LogmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while (0)

#define LOG_ERROR(LogmsgFormat, ...) \
    do { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(ERROR); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, LogmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while (0)

#define LOG_FATAL(LogmsgFormat, ...) \
    do { \
        Logger &logger = Logger::instance(); \
        logger.setLogLevel(FATAL); \
        char buf[1024] = {0}; \
        snprintf(buf, 1024, LogmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while (0)

#ifdef MUDEBUG
#define LOG_DEBUG(LogmsgFormat, ...) \
    do { \
        Logger &logger = Logger::instance(); \
        char buf[1024] = {DEBUG}; \
        snprintf(buf, 1024, LogmsgFormat, ##__VA_ARGS__); \
        logger.log(buf); \
    } while (0)
#else
#define LOG_DEBUG(LogmsgFormat, ...) 
#endif

// define log level: INFO ERROR FATAL DEBUG
enum LogLevel {

    INFO,       // common data
    ERROR,      // error data
    FATAL,      // core data
    DEBUG,      // debug data

};

class Logger : noncopyable{
public:
    // get log one_instance 
    static Logger& instance();
    // set log level
    void setLogLevel(int Level);
    // write log
    void log(std::string msg);

private:
    
    int logLevel_;

};


