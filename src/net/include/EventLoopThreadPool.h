#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <string>
#include "common/include/Noncopyable.h"

namespace CppUtil
{

namespace Net
{


class EventLoop;
class EventLoopThread;

class EventLoopThreadPool : public Noncopyable
{
public:
    using ThreadInitCallback = std::function<void(EventLoop*)>;
    EventLoopThreadPool(EventLoop* baseloop, const std::string& name );
    ~EventLoopThreadPool();
    void start(const ThreadInitCallback& cb);
    EventLoop* getNextLoop();
    EventLoop* getLoopForHash(size_t hashCode);
    std::vector<EventLoop*> getAllLoops();
private:
    EventLoop* baseLoop_;
    std::string name_;
    int numThreads_;
    bool started_;
    int next_;
    std::vector<std::unique_ptr<EventLoopThread> > threads_;
    std::vector<EventLoop*> loops_;
};

}

}