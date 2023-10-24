#pragma once

#include <stdarg.h>

#include <mutex>
#include <string>
#include <string.h>
#include "common/include/Status.h"

namespace CppUtil {
namespace Common {

enum LogLevel {
  LogLevelDebug = 0x0,
  LogLevelInfo = 0x01,
  LogLevelWarn = 0x02,
  LogLevelError = 0x03
};

inline const char* baseFileName(const char* filepath) {
  const char* lastSlash = strrchr(filepath, '/');
  return nullptr == lastSlash ? filepath : lastSlash + 1;
}

class ILogger {
 public:
  ILogger();
  virtual ~ILogger();

 public:
  virtual Status init() = 0;
  virtual void log(LogLevel level, const char* file, const char* func, int line,
                   const char* fmt, va_list args) = 0;
  virtual void log(LogLevel level, const char* file, const char* func, int line,
                   const char* fmt, ...) = 0;
};

class FileLogger : public ILogger {
 public:
  explicit FileLogger(std::string name);
  ~FileLogger();

 public:
  virtual Status init() override;
  virtual void log(LogLevel level, const char* file, const char* func, int line,
                   const char* fmt, va_list args) override;
  virtual void log(LogLevel level, const char* file, const char* func, int line,
                   const char* fmt, ...) override;
  bool addLogHeader(char* buffer, int size, const char* level, int* psize,
                    const char* file, const char* func, int line);
  void writeLog(char* str_log, int size, int level);

 private:
  void openLogFile();

 private:
  char* logData_{nullptr};
  std::string logFileName_;
  std::mutex mtx_;
  int fd_;
  LogLevel level_;
};

extern ILogger* g_logger;
#define LOG_DEBUG(msg, ...)                                               \
  CppUtil::Common::g_logger->log(CppUtil::Common::LogLevelDebug,          \
                                 CppUtil::Common::baseFileName(__FILE__), \
                                 __FUNCTION__, __LINE__, msg, ##__VA_ARGS__);
#define LOG_INFO(msg, ...)                                                \
  CppUtil::Common::g_logger->log(CppUtil::Common::LogLevelInfo,           \
                                 CppUtil::Common::baseFileName(__FILE__), \
                                 __FUNCTION__, __LINE__, msg, ##__VA_ARGS__);
#define LOG_WARN(msg, ...)                                                \
  CppUtil::Common::g_logger->log(CppUtil::Common::LogLevelWarn,           \
                                 CppUtil::Common::baseFileName(__FILE__), \
                                 __FUNCTION__, __LINE__, msg, ##__VA_ARGS__);
#define LOG_ERROR(msg, ...)                                               \
  CppUtil::Common::g_logger->log(CppUtil::Common::LogLevelError,          \
                                 CppUtil::Common::baseFileName(__FILE__), \
                                 __FUNCTION__, __LINE__, msg, ##__VA_ARGS__);

inline void initLog(const std::string& filename) {
  g_logger = new FileLogger(filename);
}

}  // namespace Common
}  // namespace CppUtil