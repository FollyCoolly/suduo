#pragma once

#include <unistd.h>

#include <functional>
#include <thread>
#include <atomic>
#include <string>

#include "Noncopyable.h"
#include "CountDownLatch.h"

namespace suduo
{

class Thread : Noncopyable
{
public:
    using ThreadFunc = std::function<void()>;

    explicit Thread(const ThreadFunc&, const std::string& name = std::string());
    // TODO 移动构造函数
    ~Thread();

    void start();
    void join();

    bool get_is_start() const
    {
        return is_started_;
    }

private:
    void setDefaultName();

    bool is_started_;
    bool is_joined_;
    
    std::shared_ptr<std::thread> thread_;
    pid_t tid_;

    ThreadFunc func_;
    std::string name_;

    CountDownLatch latch_;

    static std::atomic_int num_created_;

};

} // namespace suduo