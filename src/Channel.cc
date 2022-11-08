#include "Channel.h"

#include "EventLoop.h"
#include "Logger.h"

using namespace suduo;

Channel::Channel(EventLoop* loop, int fd)
    : loop_(loop),
      fd_(fd),
      events_(0),
      revents_(0),
      index_(-1),
      is_tied_(false)
{
}

Channel::~Channel()
{
}

void Channel::tie(const std::shared_ptr<void>& obj)
{
    tie_ = obj;
    is_tied_ = true;
}

// 更改fd相应的事件epoll_stl
void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::remove()
{
    loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receive_time)
{
    if(is_tied_)
    {
        std::shared_ptr<void> guard = tie_.lock();
        // 如果weak_ptr提升失败，说明Channel的TcpConnection对象已经不存在了
        if(guard)
        {
            handleEventWithGuard(receive_time);
        }
    }
    else
    {
        handleEventWithGuard(receive_time);
    }
}

void Channel::handleEventWithGuard(Timestamp receive_time)
{
    LOG_INFO("channel handle Event revents: %d\n", revents_);
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        if(close_callback_)
        {
            close_callback_();
        }
    }
    if(revents_ & EPOLLERR)
    {
        if(error_callback_)
        {
            error_callback_();
        }
    }
    if(revents_ & (EPOLLIN | EPOLLPRI))
    {
        if(read_callback_)
        {
            read_callback_(receive_time);
        }
    }
    if(revents_ & EPOLLOUT)
    {
        if(write_callback_)
        {
            write_callback_();
        }
    }
}