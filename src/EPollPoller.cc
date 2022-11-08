#include "EPollPoller.h"

#include <errno.h>
#include <string.h>

#include "Logger.h"
#include "Channel.h"


using namespace suduo;

constexpr int kInitEventListSize = 16;
constexpr int kNew = -1;
constexpr int kAdded = 1;
constexpr int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop* loop)
    : Poller(loop),
      epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
      events_(kInitEventListSize)
{
    if(epollfd_ < 0)
    {
        LOG_FATAL("epoll_create error:%d \n", errno);
    }
}

EPollPoller::~EPollPoller()
{
    ::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeout_ms, ChannelList* active_channels)
{
    // 由于频繁调用poll 实际上应该用LOG_DEBUG输出日志更为合理 当遇到并发场景 关闭DEBUG日志提升效率，下面同理
    LOG_INFO("func = %s => fd total count:%lu\n", __FUNCTION__, channels_.size());
     
    int num_events = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeout_ms);
    int save_errno = errno;
    Timestamp now(Timestamp::now());

    if (num_events > 0)
    {
        LOG_INFO("%d events happened\n", num_events);
        fillActiveChannels(num_events, active_channels);
        if(num_events == events_.size())
        {
            events_.resize(events_.size() * 2);
        }
    }
    else if (num_events == 0)
    {
        LOG_DEBUG("%s timeout!\n", __FUNCTION__);
    }
    else
    {
        if(save_errno != EINTR)
        {
            errno = save_errno;
            LOG_ERROR("EPollPoller::poll() error!\n");
        }
    }
    return now;
}

void EPollPoller::updateChannel(Channel* channel)
{
    const int index = channel->index();
    LOG_INFO("func = %s => fd = %d events = %d index = %d\n", __FUNCTION__, channel->fd(), channel->events(), index);

    if(index == kNew || index == kDeleted)
    {
        if(index == kNew)
        {
            int fd = channel->fd();
            channels_[fd] = channel;
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        if(channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel* channel)
{
    int fd = channel->fd();
    channels_.erase(fd);

    LOG_INFO("func = %s => fd = %d\n", __FUNCTION__, fd);

    int index = channel->index();
    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

void EPollPoller::fillActiveChannels(int num_events, ChannelList* active_channels) const
{
    for(int i = 0; i < num_events; ++i)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        active_channels->push_back(channel);
    }
}

void EPollPoller::update(int operation, Channel* channel)
{
    epoll_event event;
    ::bzero(&event, sizeof(event));

    int fd = channel->fd();

    event.events = channel->events();
    // man epoll_wait里可以查看epoll_event结构
    event.data.ptr = channel;

    if(::epoll_ctl(epollfd_, operation, fd, &event) < 0)
    {
        if(operation == EPOLL_CTL_DEL)
        {
            LOG_ERROR("epoll_ctr del error:%d\n", errno);
        } 
        else if(operation == EPOLL_CTL_ADD)
        {
            LOG_FATAL("epoll_ctr add error:%d\n", errno);
        } 
        else
        {
            LOG_FATAL("epoll_ctr mod error:%d\n", errno);
        }
    }
}