#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include "common/include/Logger.h"

namespace CppUtil
{
namespace Common
{

#define LOG_MAX_SIZE 8192
#define LOG_TAIL '\n'
#define LOG_TAIL_SIZE 1

ILogger::ILogger() = default;
ILogger::~ILogger() = default;

FileLogger::FileLogger(std::string logName)
  : level_(LogLevelDebug),
    logData_(new char[LOG_MAX_SIZE]),
    fd_(-1),
    logFileName_(logName)
{

}

FileLogger::~FileLogger()
{
    delete[] logData_;
    if(fd_ > 0){
        ::close(fd_);
        fd_ = -1;
    }
}

Status FileLogger::init()
{
    return {};
}

void FileLogger::openLogFile()
{
    if (logFileName_.empty()) {
        return;
    }

    fd_ = ::open(logFileName_.c_str(),
        O_RDWR | O_CREAT | O_APPEND,
        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH
    );
}

bool FileLogger::addLogHeader(char* buffer, int size, const char* level, int* psize, const char* file, const char*func, int line)
{
    // clock time
    timeval tv;
    if (gettimeofday(&tv, NULL) == -1) {
        return false;
    }
    
    // to calendar time
    struct tm now;

    if (localtime_r(&tv.tv_sec, &now) == NULL) {
        return false;
    }

    int written = -1;
    written = snprintf(buffer, size,
    "[%d-%02d-%02d %02d:%02d:%02d.%03d][%s][%d][%s][%s:%d] ",
    1900 + now.tm_year, 1 + now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec, (int)(tv.tv_usec / 1000),
    level, getpid(), func, file, line);

    if (written <= 0 || written >= size) {
        return false;
    }
    
    // write the header size.
    *psize = written;

    return true;
}

void FileLogger::log(LogLevel level, const char* file, const char*func, int line, const char* fmt, ...) 
{
    va_list args;
    va_start(args, fmt);
    log(level, file,func,line, fmt, args);
    va_end(args);
}

void FileLogger::log(LogLevel level, const char* file, const char*func, int line, const char* fmt, va_list args)
{
    if (level < level_ ) {
        return;
    }
    std::unique_lock<std::mutex> lk(mtx_);
    int size = 0;
    bool ok = addLogHeader(logData_, LOG_MAX_SIZE, logLevelString[level_], &size, file, func, line);
    if (!ok) {
        return;
    }
    int r0 = vsnprintf(logData_ + size, LOG_MAX_SIZE - size, fmt, args);
    if (r0 <= 0 || r0 >= LOG_MAX_SIZE - size) {
        return;
    }
    size += r0;

    // Add errno and strerror() if error. Check size to avoid security issue https://github.com/ossrs/srs/issues/1229
    if (level_ == LogLevelError && errno != 0 && size < LOG_MAX_SIZE) {
        r0 = snprintf(logData_ + size, LOG_MAX_SIZE - size, "(%s)", strerror(errno));

        // Something not expected, drop the log.
        if (r0 <= 0 || r0 >= LOG_MAX_SIZE - size) {
            return;
        }
        size += r0;
    }

    writeLog(logData_, size, level_);
}

void FileLogger::writeLog(char *str_log, int size, int level)
{
    size = std::min(LOG_MAX_SIZE - 1 - LOG_TAIL_SIZE, size);
    
    // add some to the end of char.
    str_log[size++] = LOG_TAIL;
    
    // open log file. if specified
    if (fd_ < 0) {
        openLogFile();
    }
    
    // write log to file.
    if (fd_ > 0) {
        ::write(fd_, str_log, size);
    }
}


}
}