#pragma once

#include <vector>
#include <unordered_map>

#include "Noncopyable.h"
#include "Timestamp.h"

namespace suduo
{

class Channel;
class EventLoop;

class Poller
{
public:
    using ChannelList = std::vector<Channel*>;

    Poller(EventLoop* loop) : owner_loop_(loop) {};
    virtual ~Poller() = default;

    virtual Timestamp poll(int timeout_ms, ChannelList* active_channels) = 0;
    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;

    bool hasChannel(Channel* channel) const;

    // EventLoop可以通过该接口获取默认的IO复用的具体实现
    static Poller* newDefaultPoller(EventLoop* loop);

protected:
    // 将fd映射到对应的Channel
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap channels_;

private:
    EventLoop* owner_loop_;
};


} // namespace suduo
