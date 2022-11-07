#pragma once

#include <sys/syscall.h>
#include <unistd.h>

namespace suduo
{

namespace CurrentThread
{
    // 将tid缓存下来，避免每次使用tid()时都进行系统调用的开销
    extern __thread int t_cached_tid;

    void cacheTid();

    inline int tid()
    {
        // __builtin_expect是gcc引入的，允许程序员将最有可能执行的分支告诉给编译器，这里意思是tid很可能已经缓存。
        // 通过这种方式，编译器在编译过程中，会将可能性更大的代码紧跟着起面的代码，从而减少指令跳转带来的性能上的下降。
        if(__builtin_expect(t_cached_tid == 0, 0))
        {
            cacheTid();
        }
        return t_cached_tid;
    }

} // namespace CurrentThread

} // namespace suduo