#include "Thread.h"

#include "CurrentThread.h"

using namespace suduo;

std::atomic_int Thread::num_created_;

Thread::Thread(const ThreadFunc& func, const std::string& name)
    : is_started_(false),
      is_joined_(false),
      tid_(0),
      func_(func),
      name_(name),
      latch_(1)
{
    setDefaultName();
}

Thread::~Thread()
{
    if(is_started_ && !is_joined_)
    {
        thread_->detach();
    }
}

void Thread::start()
{
    assert(!is_started_);
    is_started_ = true;

    thread_ = std::shared_ptr<std::thread>(new std::thread([&]() {
        tid_ = CurrentThread::tid();
        latch_.countDown();
        func_();
    }));

    latch_.wait();
}

void Thread::join()
{
    is_joined_ = true;
    thread_->join();
}

void Thread::setDefaultName()
{
    int num = ++num_created_;
    if(name_.empty())
    {
        char buf[32] = {0};
        snprintf(buf, sizeof(buf), "THread%d", num);
        name_ = buf;
    }
}
