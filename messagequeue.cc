#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

template <class T>
class MessageQueue {
private:
    std::queue<T> queue_;
    mutable std::mutex mtx_;  
    std::condition_variable cv_;
    std::atomic<size_t> size_ = 0; // 原子变量

public:
    // 添加消息到队列
    void push(const T& msg) {
        {
            std::unique_lock<std::mutex> lock(mtx_);
            queue_.push(msg);
        }
        size_++;
        cv_.notify_one(); // 通知等待的线程
    }

    // 尝试非阻塞地取消息
    bool try_pop(T& msg) {
        std::unique_lock<std::mutex> lock(mtx_);
        if (!queue_.empty()) {
            msg = queue_.front();
            queue_.pop();
            size_--;
            return true;
        }
        return false;
    }

    // 阻塞等待并取消息
    void blocking_pop(T& msg) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this]() { return !queue_.empty(); }); // 避免虚假唤醒
        msg = queue_.front();
        queue_.pop();
        size_--;
    }

    // 获取队列大小（线程安全）
    size_t size() const {
        return size_.load(); // 高效获取原子变量
    }
};
