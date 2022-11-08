#pragma once

#include <sys/epoll.h>

#include <vector>

#include "Poller.h"
#include "Timestamp.h"

namespace suduo
{

class Channel;

class EPollPoller : public Poller
{
public:
    EPollPoller(EventLoop* loop);
    ~EPollPoller();

    Timestamp poll(int timeout_ms, ChannelList* active_channels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;

private:
    void fillActiveChannels(int numEvents, ChannelList* active_channels) const;
    void update(int opreation, Channel* channel);
    
    using EventList = std::vector<epoll_event>;

    int epollfd_;
    EventList events_;
};



} // namespace suduo
