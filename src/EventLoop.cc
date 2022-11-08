#include "EventLoop.h"

#include "Poller.h"
#include "Channel.h"

using namespace suduo;

// Poller的超时事件为10秒钟
constexpr int kPollTimeMs = 10000;

__thread EventLoop* t_loop_in_this_thread = nullptr;

EventLoop::EventLoop()
    : is_looping_(false),
      is_quit_(false),
      threadId_(CurrentThread::tid()),
      poller_(Poller::newDefaultPoller(this))
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
    is_quit_ = false;

    while (!is_quit_)
    {
        active_channels_.clear();
        poll_return_time_ = poller_->poll(kPollTimeMs, &active_channels_);
        for(Channel* channel : active_channels_)
        {
            channel->handleEvent(poll_return_time_);
        }
    }

    LOG_INFO("EventLoop %p stop looping", this);
    is_looping_ = false;
}

void EventLoop::quit()
{
    is_quit_ = true;
}

void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL("EventLoop::abortNotInLoopThread - EventLoop %p was created in thread%d, but current thread id is %d\n", 
              this, threadId_, CurrentThread::tid());
}

bool EventLoop::hasChannel(Channel* channel)
{
    assert(channel->get_owner_loop() == this);
    assertInLoopThread();
    return poller_->hasChannel(channel);
}

void EventLoop::updateChannel(Channel* channel)
{
    assert(channel->get_owner_loop() == this);
    assertInLoopThread();
    return poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    assert(channel->get_owner_loop() == this);
    assertInLoopThread();
    return poller_->removeChannel(channel);
}