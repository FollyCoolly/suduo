#pragma once

#include <unistd.h>
#include <assert.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "Timestamp.h"
#include "Noncopyable.h"


#define LOG_DEBUG(logmsgFormat, ...)                                                \
    do                                                                              \
    {                                                                               \
        suduo::log::Logger& logger = suduo::log::Logger::get_logger();              \
        char buf[1024] = {0};                                                       \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);                           \
        logger(suduo::log::LogDst::CONSOLE, suduo::log::LogLevel::DEBUG)<< buf;                                    \
    } while (0)

#define LOG_INFO(logmsgFormat, ...)                                                 \
    do                                                                              \
    {                                                                               \
        suduo::log::Logger& logger = suduo::log::Logger::get_logger();              \
        char buf[1024] = {0};                                                       \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);                           \
        logger(suduo::log::LogDst::CONSOLE, suduo::log::LogLevel::INFO) << buf;                                    \
    } while (0)

#define LOG_WARN(logmsgFormat, ...)                                                 \
    do                                                                              \
    {                                                                               \
        suduo::log::Logger& logger = suduo::log::Logger::get_logger();              \
        char buf[1024] = {0};                                                       \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);                           \
        logger(suduo::log::LogDst::CONSOLE, suduo::log::LogLevel::WARN) << buf;                                    \
    } while (0)

#define LOG_ERROR(logmsgFormat, ...)                                                \
    do                                                                              \
    {                                                                               \
        suduo::log::Logger& logger = suduo::log::Logger::get_logger();              \
        char buf[1024] = {0};                                                       \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);                           \
        logger(suduo::log::LogDst::CONSOLE, suduo::log::LogLevel::ERROR) << buf;                                    \
    } while (0)

#define LOG_FATAL(logmsgFormat, ...)                                                \
    do                                                                              \
    {                                                                               \
        suduo::log::Logger& logger = suduo::log::Logger::get_logger();              \
        char buf[1024] = {0};                                                       \
        snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__);                           \
        logger(suduo::log::LogDst::CONSOLE, suduo::log::LogLevel::FATAL) << buf;                                    \
    } while (0)


namespace suduo
{

namespace log
{
enum LogLevel
{
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL
};

enum LogDst
{
    CONSOLE,
    FILE,
    ALL
};

class TmpStream : public std::ostringstream
{
public:
    ~TmpStream()
    {
        // 将流转为字符串
        std::string s = this->str();
        if(s.length() != 0)
        {
            if(log_level_ >= least_log_level_)
            {
                std::lock_guard<std::mutex> lcgd(mt_);
                switch (log_level_)
                {
                    case LogLevel::DEBUG:
                    {
                        dst_stream_ << "[DEBUG]";
                        break;
                    }

                    case LogLevel::INFO:
                    {
                        dst_stream_ << "[INFO]";
                        break;
                    }

                    case LogLevel::WARN:
                    {
                        dst_stream_ << "[WARN]";
                        break;
                    }

                    case LogLevel::FATAL:
                    {
                        dst_stream_ << "[FATAL]";
                        break;
                    }
                    
                    default:
                    {
                        assert(false);
                    }
                }
                dst_stream_ << Timestamp::now().toString() << " : ";
                dst_stream_ << this->str() << std::endl;
            }
        }
    }

private:
    friend class Logger;

    static std::mutex mt_;


    std::stringstream& dst_stream_;
    LogLevel log_level_;
    LogLevel least_log_level_;
    
    TmpStream(std::stringstream& dst_stream, LogLevel log_level, LogLevel least_log_level)
              : dst_stream_(dst_stream), log_level_(log_level), least_log_level_(least_log_level) {}
    
    TmpStream(TmpStream& ts)
              : dst_stream_(ts.dst_stream_), log_level_(ts.log_level_), least_log_level_(ts.least_log_level_) {}
    

};

class Logger : Noncopyable
{
public:
    ~Logger()
    {
        this->is_stop_ = true;
        this->flush_time_ = 0;
        this->logger_condition_.notify_all();
        this->flush_thread.join();
    }


    // 取得单例
    static Logger& get_logger();

    LogLevel get_log_level();
    LogLevel set_log_level(LogLevel new_log_level);

    TmpStream operator ()(LogDst log_dst, LogLevel log_level = LogLevel::DEBUG);

    void flush(LogDst log_dst = LogDst::ALL);
    int get_flush_time();
    int set_flush_time(int new_flush_time);
    bool set_dst_file(std::string new_dst_file);
    std::string get_dst_file();


private:
    friend class TmpStream;

    // 单例
    static Logger logger_;

    // buffer
    std::stringstream console_stream_;
    std::stringstream file_stream_;

    LogLevel log_level_;

    // 输出的目标文件
    std::string dst_file_;

    // 锁
    std::mutex logger_lock_;
    std::condition_variable logger_condition_;
    
    std::atomic<int> flush_time_;
    std::atomic<bool> is_stop_;

    std::thread flush_thread;

    

    Logger(): log_level_(LogLevel::DEBUG), dst_file_("./log.txt"), flush_time_(1000), is_stop_(false)
    {
        this->console_stream_.str("");
        this->file_stream_.str("");
        this->flush_thread = std::thread([this]()
        {
            while(!this->is_stop_)
            {
                std::unique_lock<std::mutex> ul(this->logger_lock_);
                this->logger_condition_.wait_for(ul, std::chrono::milliseconds(this->flush_time_));
                ul.unlock();
                this->flush();
            }
        });
    }

};





} // namespace log

} // namespace suduo