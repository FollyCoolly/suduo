#pragma once

#include <sys/epoll.h>

#include <functional>
#include <memory>

#include "Noncopyable.h"
#include "Timestamp.h"

namespace suduo
{

class EventLoop;

class Channel : Noncopyable
{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(Timestamp)>;

    Channel(EventLoop* loop, int fd);
    ~Channel();

    void handleEvent(Timestamp receive_time);
    void setReadCallBack(const ReadEventCallback& cb)
    {
        read_callback_ = cb;
    }

    void setWriteCallBack(const EventCallback& cb)
    {
        write_callback_ = cb;
    }

    void setCloseCallBack(const EventCallback& cb)
    {
        close_callback_ = cb;
    }
    void setErrorCallBack(const EventCallback& cb)
    {
        error_callback_ = cb;
    }

    int fd() const
    {
        return fd_;
    }

    int events() const{
        return events_;
    }

    void set_revents(int revents)
    {
        revents_ = revents;
    }

    // 保证Channel还在执行回调操作时不会被remove
    void tie(const std::shared_ptr<void>& obj);

    // 设置fd相应的事件状态
    void enableReading()
    {
        events_ |= kReadEvent;
        update();
    }

    void enableWriting()
    {
        events_ |= kWriteEvent;
        update();
    }

    void disbleReading()
    {
        events_ &= ~kReadEvent;
        update();
    }

    void disableWriting()
    {
        events_ &= ~kWriteEvent;
        update();
    }

    void disbleAll()
    {
        events_ = kNoneEvent;
        update();
    }

    // 获取fd当前的事件状态
    bool isNoneEvent() const
    {
        return events_ == kNoneEvent;
    }

    bool isReadEvent() const
    {
        return events_ & kReadEvent;
    }

    bool isWriteEvent() const
    {
        return events_ & kWriteEvent;
    }

    int index()
    {
        return index_;
    }

    void set_index(int index)
    {
        index_ = index;
    }

    EventLoop* get_owner_loop()
    {
        return loop_;
    }

    void remove();

private:
    void update();
    void handleEventWithGuard(Timestamp receive_time);

    static constexpr int kNoneEvent = 0;
    static constexpr int kReadEvent = EPOLLIN | EPOLLPRI;
    static constexpr int kWriteEvent = EPOLLOUT;

    // 具体事件的回调
    ReadEventCallback read_callback_;
    EventCallback write_callback_;
    EventCallback close_callback_;
    EventCallback error_callback_;

    // Poller监听的对象
    EventLoop* loop_;
    const int fd_;
    int events_;
    int revents_;
    int index_;

    std::weak_ptr<void> tie_;
    bool is_tied_;
};

} // namespace suduo
