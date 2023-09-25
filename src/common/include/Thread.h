#ifndef CPP_UTIL_COMMON_THREAD_H
#define CPP_UTIL_COMMON_THREAD_H

#include <memory>
#include <functional>
#include <string>
#include "common/include/Noncopyable.h"

namespace CppUtil
{

class Thread : public Noncopyable
{
public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(ThreadFunc th, const std::string& name);
    ~Thread();

    void start();
    void stop();

    bool started() const;
    pid_t tid() const;

    const std::string& name();
    void exec();
private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

} // namespace CppUtil

#endif // CPP_UTIL_THREAD_H