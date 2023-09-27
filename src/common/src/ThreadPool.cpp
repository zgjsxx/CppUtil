#include <vector>
#include <deque>
#include <atomic>
#include <iostream>
#include <condition_variable>
#include <assert.h>
#include "common/include/ThreadPool.h"
#include "common/include/Thread.h"
#include "common/include/CountDownLatch.h"

namespace CppUtil
{
struct ThreadPool::Impl
{
    Impl(const std::string& name)
        : name_(name)
    {

    }
    bool isFull(){
        return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
    }

    std::vector<std::unique_ptr<Thread>> threads_;
    std::deque<Task> queue_;
    std::condition_variable emptyCond_;
    std::condition_variable fullCond_;
    mutable std::mutex mtx_;
    std::string name_;
    std::atomic_bool running_{false};
    size_t maxQueueSize_{0};
};

ThreadPool::ThreadPool(const std::string& name)
  : impl_(std::make_unique<Impl>(name))
{

}

ThreadPool::~ThreadPool()
{
    if(impl_->running_)
    {
        stop();
    }
}

void ThreadPool::start(int num)
{
    impl_->running_ = true;
    impl_->threads_.reserve(num);
    for(int i = 0; i < num; ++i){
        impl_->threads_.emplace_back(std::make_unique<Thread>(std::bind(&ThreadPool::execTask, this), impl_->name_ + std::to_string(i)));
        impl_->threads_[i]->start();
    }
}

//stop and drop the jobs in the queue
void ThreadPool::stop()
{
    impl_->running_ = false;
    impl_->emptyCond_.notify_all();
    impl_->fullCond_.notify_all();
    for(auto& th : impl_->threads_){
        th->stop();
    }
}

//stop and exec the extra jobs in the queue
void ThreadPool::stopSafe()
{
    impl_->running_ = false;
    impl_->emptyCond_.notify_all();
    impl_->fullCond_.notify_all();
    for(auto& th : impl_->threads_){
        th->stop();
    }
    {
        std::unique_lock<std::mutex> lk(impl_->mtx_);
        while(!impl_->queue_.empty()){
            auto task = impl_->queue_.front();
            impl_->queue_.pop_front();
            task();           
        }
    }
}

ThreadPool::Task ThreadPool::getTask()
{
    std::unique_lock<std::mutex> lk(impl_->mtx_);
    impl_->emptyCond_.wait(lk, [this](){return !impl_->queue_.empty() || !impl_->running_;});
    Task task;
    if(impl_->running_)
    {
        task = impl_->queue_.front();
        impl_->queue_.pop_front();
        if(impl_->maxQueueSize_ > 0){
            impl_->fullCond_.notify_one();
        }
    }

    return task;
}

void ThreadPool::postTask(ThreadPool::Task task)
{
    if(impl_->threads_.empty()){
        task(); //run it directly
        return;
    }

    std::unique_lock<std::mutex> lk(impl_->mtx_);       
    impl_->fullCond_.wait(lk, [this](){return !impl_->isFull()  || !impl_->running_;});

    if(!impl_->running_) return;

    impl_->queue_.emplace_back(std::move(task));
    impl_->emptyCond_.notify_one();
}

void ThreadPool::execTask()
{
    while(impl_->running_){
        Task task = getTask();
        if(task){
            task();
        }
    }
}

} // namespaec CppUtil