#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <iostream>
#include <chrono>

class ThreadPool
{
private:
    void workerThread(){
        while (true)
        {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(queuemutex);
                condition.wait(lock,[this](){
                    return this->stopFlag||!this->tasks.empty();
                });
                if (this->stopFlag && this->tasks.empty())
                {
                    return;
                }
                task=std::move(this->tasks.front());
                this->tasks.pop();
            }
            task();
        }
        
    }

    std::vector<std::thread> threads;        // 线程集合
    std::queue<std::function<void()>> tasks; // 任务队列
    std::mutex queuemutex;                   // 任务队列的互斥锁
    std::condition_variable condition;       // 条件变量
    std::atomic<bool> stopFlag;              // 停止标志
public:
    ThreadPool(int threadCount) : stopFlag(false)
    {
        for (int i = 0; i < threadCount; i++)
        {
            threads.emplace_back(std::thread(&ThreadPool::workerThread, this)); // 类成员函数
        }
    }

    // 添加任务队列倒线程池
    template <typename F, typename... Args>
    void enqueueTask(F &&func, Args &&...args)
    {
        {
            std::unique_lock<std::mutex> lock(queuemutex);
            tasks.emplace(std::bind(std::forward<F>(func), std::forward<Args>(args)...));
        }
        condition.notify_one(); // 唤醒一个线程处理任务
    }

    ~ThreadPool()
    {
        {
            std::unique_lock<std::mutex> lock(queuemutex);
            stopFlag = true;
        }
        condition.notify_all();
        for (std::thread& thread:threads)
        {
            if (thread.joinable())
            {
                thread.join();
            }
        }
    }
};
