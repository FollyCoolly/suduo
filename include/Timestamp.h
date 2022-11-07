#pragma once

#include <string>

namespace suduo
{

class Timestamp
{
public:
    Timestamp();
    explicit Timestamp(int64_t microSecondsSinceEpoch_);
    static Timestamp now();
    std:: string toString() const;

private:
    int64_t microSecondsSinceEpoch_;
};

} // namespace suduo