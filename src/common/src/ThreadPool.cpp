#include <vector>
#include <deque>
#include "common/include/ThreadPool.h"
#include "common/include/Thread.h"

namespace CppUtil
{
struct ThreadPool::Impl
{
    std::vector<std::unique_ptr<Thread>> threads_;
    std::deque<Task> queue_;
};


void ThreadPool::start(int num)
{
    impl_->threads_.reserve(num);
    for(int i = 0; i < num; ++i){
        
    }
}


ThreadPool::Task ThreadPool::getTask()
{
    while(impl_->queue_.empty()){

    }
    Task task;
    if(!impl_->queue_.empty()){
        task = impl_->queue_.front();
        impl_->queue_.pop_front();

    }
}

void ThreadPool::postTask(ThreadPool::Task task)
{

}

} // namespaec CppUtil