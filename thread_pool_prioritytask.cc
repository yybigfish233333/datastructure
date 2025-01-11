#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>
#include <mutex>
#include <queue>

class thread_pool
{
public:
    static const int kInitThreadSize = 3; // 线程数量
    enum taskPriorityE
    {
        level0,
        level1,
        level2
    };
    typedef std::function<void()> Task;
    typedef std::pair<taskPriorityE, Task> TaskPair;

    thread_pool():m_isStarted(false){}
    ~thread_pool(){
        if (m_isStarted)
        {
            stop();
        }
        
    }

    void start(){
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_isStarted)
        {
            return;//防止重复启动
        }
        m_isStarted=true;
        for (int i = 0; i < kInitThreadSize; i++)
        {
            m_threads.emplace_back(new std::thread(&thread_pool::threadLoop,this));
        }
    }
    void stop(){
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_isStarted=false;
        }
        m_cond.notify_all();//通知所有等待的线程结束
        for (auto &thread:m_threads)
        {
            if (thread->joinable())
            {
                thread->join();
            }
            delete thread;
        }
        m_threads.clear();
    }
    void addTask(const Task& task){
        std::unique_lock<std::mutex> lock(m_mutex);
        TaskPair taskPair(level2,task);
        m_tasks.push(taskPair);
        m_cond.notify_one();
    }
    void addTask(const TaskPair &taskPair){
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_tasks.push(taskPair);
        }
        m_cond.notify_one();
    }

private:
    thread_pool(const thread_pool &) = delete; // 禁止拷贝
    const thread_pool &operator=(const thread_pool &) = delete;
    struct TaskPriorityCmp
    {
        bool operator()(const thread_pool::TaskPair p1, const thread_pool::TaskPair p2)
        {
            return p1.first > p2.first; // first小的优先
        }
    };

    void threadLoop(){
        while (m_isStarted)
        {
            Task task=take();
            if (task)
            {
                task();
            }
            
        }
        
    }
    Task take(){
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_tasks.empty() && m_isStarted)
        {
            m_cond.wait(lock);//阻塞等待任务
        }
        if (!m_isStarted)
        {
            return nullptr;//线程池停止，返回空任务
        }
        Task task=m_tasks.top().second;
        m_tasks.pop();
        return task;
    }

    typedef std::vector<std::thread *> Threads;
    typedef std::priority_queue<TaskPair, std::vector<TaskPair>, TaskPriorityCmp> Tasks;

    Threads m_threads;
    Tasks m_tasks;

    std::mutex m_mutex;
    std::condition_variable m_cond;
    bool m_isStarted;
};

