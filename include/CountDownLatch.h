#pragma once

#include <assert.h>

#include <mutex>
#include <condition_variable>

#include "Noncopyable.h"

namespace suduo
{

class CountDownLatch : Noncopyable
{
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;
private:
    int count_;
    std::condition_variable cv_;
    mutable std::mutex mutex_;
};

} // namespace suduo

