#pragma once

#include <iostream>

class Timestamp {

public:
    
    Timestamp();
    explicit Timestamp(int64_t microSecondSinceEpoth_);
    static Timestamp now();
    std::string toString() const;

private:
    int64_t microSecondSinceEpoth_;
};
