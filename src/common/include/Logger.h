#pragma once

#include <stdarg.h>
#include <mutex>
#include <string>
#include "common/include/Status.h"

namespace CppUtil
{
namespace Common
{

enum LogLevel
{
    LogLevelDebug = 0x0,
    LogLevelInfo = 0x01,
    LogLevelWarn = 0x02,
    LogLevelError = 0x03
};

const char* logLevelString[] = {
    "Debug", "Info", "Warn", "Error"
};

class ILogger
{
public:
    ILogger();
    virtual ~ILogger();
public:
    virtual Status init() = 0;
    virtual void log(LogLevel level, const char* file, const char*func, int line, const char* fmt, va_list args) = 0;
    virtual void log(LogLevel level, const char* file, const char*func, int line, const char* fmt, ...) = 0;
};


class FileLogger : public ILogger
{
public:
    explicit FileLogger(std::string name);
    ~FileLogger();

public:
    virtual Status init() override;
    virtual void log(LogLevel level, const char* file, const char*func, int line, const char* fmt, va_list args) override;
    virtual void log(LogLevel level, const char* file, const char*func, int line, const char* fmt, ...) override;
    bool addLogHeader(char* buffer, int size, const char* level, int* psize, const char* file, const char*func, int line);
    void writeLog(char *str_log, int size, int level);
private:
    void openLogFile();

private:
    char* logData_{nullptr};
    std::string logFileName_;
    std::mutex mtx_;
    int fd_;
    LogLevel level_;

};

}
}