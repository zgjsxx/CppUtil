#include "common/include/CountDownLatch.h"

namespace CppUtil
{

CountDownLatch::CountDownLatch(const size_t count): count_(count){}

void CountDownLatch::wait()
{
    std::unique_lock<std::mutex> lk(mtx_);
    if(count_ > 0){
        cv_.wait(lk, [this](){return count_ == 0;});
    }
}

void CountDownLatch::countDown()
{
    std::unique_lock<std::mutex> lk(mtx_);
    if(count_ > 0){
        count_--;
        cv_.notify_all();
    }
}

size_t CountDownLatch::getCount()
{
    std::unique_lock<std::mutex> lk(mtx_);
    return count_;
}

void CountDownLatch::setCount(int num)
{
    std::unique_lock<std::mutex> lk(mtx_);
    count_ = num;
}

}