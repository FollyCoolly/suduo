#include <Poller.h>
#include <EPollPoller.h>

using namespace suduo;

Poller* Poller::newDefaultPoller(EventLoop* loop)
{
    return new EPollPoller(loop);
}