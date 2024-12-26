#include <queue>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <iostream>
#include <thread>
#include <algorithm>
#include <functional>

template <typename T>
class BlockingQueue
{

public:
    BlockingQueue(size_t capacity) : capacity_(capacity) {}

    void push(const T &data)
    {
        std::unique_lock<std::mutex> lock(mu_);
        // 阻塞push，当队列满时
        while (queue_.size() >= capacity_)
        {
            std::cout << "queue full,stop push" << std::endl;
            std::cout << "queue size:" << queue_.size() << std::endl;
            // 打印一下队列此时的元素有哪些
            std::queue<T> tempQueue = queue_;
            while (!tempQueue.empty())
            {
                std::cout << tempQueue.front() << " ";
                tempQueue.pop();
            }
            std::cout << std::endl;
            fullCond_.wait(lock);
        }
        queue_.push(data);
        std::cout << "push: " << data << " successfully pushed into the queue." << std::endl; // 新增日志

        emptyCond_.notify_one(); // 队列不为空时，通知pop，说明可以pop了
    }
    size_t size()
    {
        std::unique_lock<std::mutex> lock(mu_); // 获得锁
        return queue_.size();
    }
    T pop()
    {
        std::unique_lock<std::mutex> lock(mu_);
        while (queue_.empty())
        {
            // 如果队列为空了，说明不能pop了，需要阻塞pop
            std::cout << "queue empty,stop pop" << std::endl;
            emptyCond_.wait(lock);
        }
        auto value = queue_.front();
        queue_.pop();
        fullCond_.notify_one(); // 队列不满时，通知push，说明可以push了
        return value;
    }

private:
    std::condition_variable emptyCond_;
    std::condition_variable fullCond_;
    std::mutex mu_;
    std::queue<T> queue_;
    size_t capacity_;
};

int main()
{
    std::mutex printMu;
    BlockingQueue<int> queue(2);
    auto push = [&queue, &mu = printMu](std::vector<int> data)
    {
        for (auto num : data)
        {
            {
                std::lock_guard<std::mutex> lock(mu);
                std::cout << "std::this_thread::get_id():" << std::this_thread::get_id() << " push:" << num << std::endl;
            }
            queue.push(num);
        }
    };
    auto pop = [&queue, &mu = printMu](size_t count)
    {
        {
            std::lock_guard<std::mutex> lock(mu);
            std::cout << std::this_thread::get_id() << ": wait for 1s, then start pop" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1)); // 模拟处理时间
        while (count--)
        {
            auto value = queue.pop();
            {
                std::lock_guard<std::mutex> lock(mu);
                std::cout << "std::this_thread::get_id():" << std::this_thread::get_id() << " pop value:" << value << std::endl;
            }
        }
    };

    std::thread t1(std::bind(push, std::vector<int>{1, 2, 3, 4}));
    std::thread t2(std::bind(pop, 8));
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::thread t3(std::bind(push, std::vector<int>{5, 6, 7, 8}));

    t1.join();
    t2.join();
    t3.join();
    return 0;
}
