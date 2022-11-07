#pragma once

#include <poll.h>

#include <atomic>

#include "CurrentThread.h"
#include "Noncopyable.h"
#include "Logger.h"




namespace suduo
{

class EventLoop : Noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void loop();

    void assertInLoopThread()
    {
        if(!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }

    // 判断是否在当前EventLoop对象创建时的线程里
    bool isInLoopThread() const
    {
        printf("isInLoopThread() : pid = %d, tid = %d threadId = %d\n", getpid(), CurrentThread::tid(), threadId_);
        return threadId_ == CurrentThread::tid();
    }

    static EventLoop* getEventLoopOfCurrentThread();

private:
    void abortNotInLoopThread();

    std::atomic_bool is_looping_;
    
    const pid_t threadId_; // EventLoop创建时的线程id
};

} // namespace suduo

