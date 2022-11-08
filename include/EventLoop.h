#pragma once

#include <poll.h>

#include <atomic>
#include <vector>

#include "CurrentThread.h"
#include "Noncopyable.h"
#include "Logger.h"




namespace suduo
{

class Channel;
class Poller;

class EventLoop : Noncopyable
{
public:
    EventLoop();
    ~EventLoop();

    void loop();
    void quit();

    void assertInLoopThread()
    {
        if(!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }

    // 通过EventLoop作为桥梁管理Poller与Channel的关系
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    // 判断是否在当前EventLoop对象创建时的线程里
    bool isInLoopThread() const
    {
        printf("isInLoopThread() : pid = %d, tid = %d threadId = %d\n", getpid(), CurrentThread::tid(), threadId_);
        return threadId_ == CurrentThread::tid();
    }

    static EventLoop* getEventLoopOfCurrentThread();

private:
    void abortNotInLoopThread();

    // 此部分以后或许可以进行解耦
    using ChannelList = std::vector<Channel*>;

    std::atomic_bool is_looping_;
    std::atomic_bool is_quit_;
    
    const pid_t threadId_; // EventLoop创建时的线程id

    std::unique_ptr<Poller> poller_;
    ChannelList active_channels_;

    Timestamp poll_return_time_;
};

} // namespace suduo

