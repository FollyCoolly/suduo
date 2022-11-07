#include "CountDownLatch.h"

using namespace suduo;

CountDownLatch::CountDownLatch(int count) : count_(count) {}

void CountDownLatch::wait()
{
    std::unique_lock<std::mutex> lck(mutex_);
    while(count_ > 0)
    {
        cv_.wait(lck);
    }
}

void CountDownLatch::countDown()
{
    std::unique_lock<std::mutex> lck(mutex_);
    assert(count_ > 0);
    --count_;
    if(0 == count_)
    {
        cv_.notify_all();
    }
}

int CountDownLatch::getCount() const
{
    std::unique_lock<std::mutex> lck(mutex_);
    return count_;
}