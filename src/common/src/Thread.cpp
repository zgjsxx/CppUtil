#include <thread>
#include <sys/prctl.h>
#include <atomic>
#include "common/include/Thread.h"


namespace CppUtil
{

thread_local int t_tid = 0;

struct Thread::Impl
{
  Impl(ThreadFunc func, const std::string name)
  : func_(std::move(func)),
    started_(false),
    tid_(0),
    name_(name)
  {
    
  }

  std::atomic_bool         started_{false};
  pid_t        tid_{0};
  ThreadFunc   func_;
  std::string  name_;
  std::unique_ptr<std::thread> threadPtr_{nullptr};
}; 

Thread::Thread(ThreadFunc func, const std::string& name):
    impl_(std::make_unique<Impl>(func, name))
{

}

Thread::~Thread()
{
    if(impl_->started_)
    {
        stop();
    }
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
    if(!impl_->name_.empty()){
        ::prctl(PR_SET_NAME, impl_->name_);
    }

    impl_->func_();
}

void Thread::stop()
{
    if(nullptr != impl_->threadPtr_ && impl_->threadPtr_->joinable()){
        impl_->started_ = false;
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
    return 0;
}

const std::string& Thread::name() const
{
    return impl_->name_;
}

} // CppUtil