#define CATCH_CONFIG_MAIN

#include <thread>
#include "catch.hpp"
#include "common/include/Thread.h"
#include "common/include/ThreadPool.h"
#include "common/include/CountDownLatch.h"

TEST_CASE("1", "ThreadPool"){
    CppUtil::ThreadPool threadpool("default");
    threadpool.start(3);
    CppUtil::CountDownLatch latch(3);
    for(int i = 0;i < 3; ++i){
        auto task = [&latch](){
            std::cout << "Hello World" << std::endl;
            latch.countDown();
        };
        threadpool.postTask(task);
    }

    latch.wait();
    threadpool.stop();
}

TEST_CASE("2", "ThreadPool"){
    CppUtil::ThreadPool threadpool("default");
    threadpool.start(3);
    for(int i = 0;i < 3; ++i){
        auto task = [](){
            std::cout << "Hello World" << std::endl;
        };
        threadpool.postTask(task);
    }

    threadpool.stopSafe();
}