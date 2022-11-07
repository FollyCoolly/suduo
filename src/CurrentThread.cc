#include "CurrentThread.h"

namespace suduo
{

namespace CurrentThread
{

    __thread int t_cached_tid = 0;

    void cacheTid()
    {
        if(t_cached_tid == 0)
        {
            t_cached_tid = static_cast<pid_t>(::syscall(SYS_gettid));
        }
    }

} // namespace CurrentThread

} // namespace suduo