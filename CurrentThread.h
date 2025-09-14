#pragma once
#include <sys/syscall.h>      /* Definition of SYS_* constants */
#include <unistd.h>

namespace CurrentThread {

    extern __thread int t_cacheTid;

    int cacheTid();

    inline int tid () { 
        if (__builtin_expect(t_cacheTid == 0, 0))
            cacheTid();
        return t_cacheTid;
    }

}
