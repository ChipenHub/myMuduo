#include "CurrentThread.h"


namespace CurrentThread {

    __thread int t_cacheTid = 0;




    int cacheTid() {
    
        // by call syscall
        if (t_cacheTid == 0) t_cacheTid = static_cast<pid_t>(::syscall(SYS_gettid));
        return t_cacheTid;

    }

}
