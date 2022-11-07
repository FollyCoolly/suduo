#include "Logger.h"

using namespace suduo::log;

std::mutex TmpStream::mt_;
Logger Logger::logger_;

Logger& Logger::get_logger()
{
    return logger_;
}

LogLevel Logger::get_log_level()
{
    std::lock_guard<std::mutex> lcgd(this->logger_lock_);
    LogLevel ret = this->log_level_;
    return ret;
}

LogLevel Logger::set_log_level(LogLevel new_log_level)
{
    std::lock_guard<std::mutex> lcgd(this->logger_lock_);
    LogLevel ret = this->log_level_;
    this->log_level_ = new_log_level;
    return ret;
}

int Logger::set_flush_time(int new_flush_time)
{
    std::lock_guard<std::mutex> lcgd(this->logger_lock_);
    int ret = this->flush_time_;
    this->flush_time_ = new_flush_time;
    return ret;
}

int Logger::get_flush_time()
{
    std::lock_guard<std::mutex> lcgd(this->logger_lock_);
    int ret = this->flush_time_;
    return ret;
}

bool Logger::set_dst_file(std::string new_dst_file)
{
    // 检查是否对new_dsr_file有写权限
    if(access(new_dst_file.c_str(), 4))
    {
        std::lock_guard<std::mutex> lcgd(this->logger_lock_);
        this->dst_file_ = new_dst_file;
        return true;
    }
    return false;
}

std::string Logger::get_dst_file()
{
    std::lock_guard<std::mutex> lcgd(this->logger_lock_);
    std::string ret = this->dst_file_;
    return ret;
}

TmpStream Logger::operator ()(LogDst log_dst, LogLevel log_level)
{
    switch(log_dst)
    {
        case LogDst::CONSOLE:
        {
            TmpStream tmp_stream(this->console_stream_, log_level, this->log_level_);
            return tmp_stream;
        }
        case LogDst::FILE:
        {
            TmpStream tmp_stream(this->file_stream_, log_level, this->log_level_);
            return tmp_stream;
        }
        default:
        {
            assert(false);
        }
    }
    
}

void Logger::flush(LogDst log_dst)
{
    std::lock_guard<std::mutex> lcgd(this->logger_lock_);
    switch (log_dst)
    {
        case LogDst::CONSOLE:
        {
            std::cout << console_stream_.str();
            console_stream_.str("");
            break;
        }

        case LogDst::FILE:
        {
            std::ofstream out;
            out.open(this->dst_file_, std::ios::app);
            if(out)
            {
                out << this->file_stream_.str();
            }
            this->file_stream_.str("");
            
            break;
        }

        case LogDst::ALL:
        {
            std::cout << console_stream_.str();
            console_stream_.str("");

            std::ofstream out;
            out.open(this->dst_file_, std::ios::app);
            if(out)
            {
                out << this->file_stream_.str();
            }
            this->file_stream_.str("");
            
            break;
        }
        
        default:
            break;
    }
}





