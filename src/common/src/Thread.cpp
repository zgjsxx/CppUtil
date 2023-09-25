#include <thread>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include "common/include/Thread.h"


namespace CppUtil
{

pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}

struct Thread::Impl
{
  Impl(ThreadFunc func, const std::string name)
  : func_(std::move(func)),
    started_(false),
    tid_(0),
    name_(name),
    joined_(0)

  {
    
  }

  bool         started_;
  bool         joined_;
  pid_t        tid_;
  ThreadFunc   func_;
  std::string  name_;
  std::unique_ptr<std::thread> threadPtr_{nullptr};
}; 

Thread::Thread(ThreadFunc func, const std::string& name):
    impl_(std::make_unique<Impl>(func, name))
{

}

void Thread::start()
{
    if(nullptr == impl_->threadPtr_){
        impl_->threadPtr_ = std::make_unique<std::thread>(&Thread::exec, this);
        impl_->started_ = true;
    }
}

void Thread::exec()
{
    ::prctl(PR_SET_NAME, impl_->name_);
    impl_->func_();
}

void Thread::stop()
{
    if(nullptr != impl_->threadPtr_ && impl_->threadPtr_->joinable()){
        impl_->threadPtr_->join();
        impl_->threadPtr_ = nullptr;
    } 
}

bool Thread::started() const
{
    return impl_->started_;
}

pid_t Thread::tid() const
{

}

const std::string& name()
{
    
}

} // CppUtil