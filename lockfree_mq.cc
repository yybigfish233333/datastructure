#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <mutex>
#include <queue>
#include <atomic>

template <typename T>
class LockFreeQueue
{
private:
    struct Element
    {
        std::atomic<bool> full_;
        T data_;
    };
    std::vector<Element> data_;
    std::atomic<size_t> read_index_;
    std::atomic<size_t> write_index_;

public:
    LockFreeQueue(size_t capacity = 1024) : data_(capacity), read_index_(0), write_index_(0) {}
    ~LockFreeQueue() {}

public:
    bool enqueue(const T& value)
    {
        size_t write_index = 0;
        Element *e = nullptr;
        do
        {
            // 要获取write_index_并且查看write_index_是否合法，最后查看对应index的缓冲区的地方有没有被占用
            write_index = write_index_.load(std::memory_order_relaxed);
            if (write_index >= (read_index_.load(std::memory_order_acquire) + data_.size()))
            {
                return false;
            }
            size_t index = write_index %  data_.size();
            e = &data_[index];
            if (e->full_.load(std::memory_order_acquire))
            {
                return false; // 说明已经被占用了，这个write_index_
            }
        } while (!write_index_.compare_exchange_weak(write_index, write_index + 1, std::memory_order_release, std::memory_order_relaxed));
        e->data_ = value;
        e->full_.store(true, std::memory_order_release);
        return true;
    }

    bool dequeue(T& value)
    {
        size_t read_index = 0;
        Element *e = nullptr;
        do
        {
            read_index = read_index_.load(std::memory_order_relaxed);
            if (read_index >= (write_index_.load(std::memory_order_acquire)))
            {
                return false;
            }
            size_t index = read_index % data_.size();
            e = &data_[index];
            if (!e->full_.load(std::memory_order_acquire))
            {
                return false;
            }

        } while (!read_index_.compare_exchange_weak(read_index, read_index + 1, std::memory_order_release, std::memory_order_relaxed));
        value = std::move(e->data_);
        e->full_.store(false, std::memory_order_release); // 表示该槽位的数据已经被读过了，所以槽位空出来，因为是环形的消息队列
        return true;
    }
};
