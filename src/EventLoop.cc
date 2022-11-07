#include "EventLoop.h"

using namespace suduo;

__thread EventLoop* t_loop_in_this_thread = nullptr;

EventLoop::EventLoop()
    : is_looping_(false),
    threadId_(CurrentThread::tid())
{
    LOG_DEBUG("EventLoop %p created in thread %d\n", this, threadId_);
    if(t_loop_in_this_thread)
    {
        LOG_FATAL("Another EventLoop %p exists in this thread %d\n", t_loop_in_this_thread, threadId_);
    }
    else
    {
        t_loop_in_this_thread = this;
    }
}

EventLoop::~EventLoop()
{
    assert(!is_looping_);
    t_loop_in_this_thread = nullptr;
}

// 每个线程至多有一个EventLoop对象，该static成员函数返回这个线程对应的EventLoop对象指针。
EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
    return t_loop_in_this_thread;
}

void EventLoop::loop()
{
    assert(!is_looping_);
    assertInLoopThread();
    is_looping_ = true;

    ::poll(NULL, 0, 5*1000);

    LOG_DEBUG("EventLoop %p stop looping", this);
    is_looping_ = false;
}

void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL("EventLoop::abortNotInLoopThread - EventLoop %p was created in thread%d, but current thread id is %d\n", 
              this, threadId_, CurrentThread::tid());
}