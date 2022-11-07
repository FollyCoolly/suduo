#pragma once

namespace suduo
{

class Noncopyable
{
public:
    Noncopyable(const Noncopyable&) = delete;
    Noncopyable& operator=(const Noncopyable &) = delete;
protected:
    // 声明成protected，可以令这个函数不能从外部被直接调用。在这里让用户不能直接构造noncopyable类，而是只能构造其子类
    Noncopyable() = default;
    ~Noncopyable() = default;
};

}