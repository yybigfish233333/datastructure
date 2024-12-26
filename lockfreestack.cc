#include <thread>
#include <atomic>   
#include <iostream>
#include <cassert>

template<typename T>    
class LockFreeStack
{
public:
    LockFreeStack():head_(nullptr){} 
    ~LockFreeStack();

    void push(const T& value);
    bool pop(T& value); 
private:
    struct Node{
        T data;
        Node* next;
        Node(const T& data):data(data),next(nullptr){}
    };
    std::atomic<Node*> head_;   

};

template<typename T>    
void LockFreeStack<T>::push(const T& value)
{
    Node* new_node=new Node(value);
    new_node->next=head_.load(std::memory_order_relaxed);//获取当前head_的值，头插法
    while (!head_.compare_exchange_weak(new_node->next,new_node,std::memory_order_release,std::memory_order_relaxed)); 
    {
        //如果head_的值没有被其他线程改变，则将new_node作为新的head_
    }
    
}   

template<typename T>
bool LockFreeStack<T>::pop(T& value)
{
    Node* old_head=head_.load(std::memory_order_relaxed);
    while (old_head && !head_.compare_exchange_weak(old_head,old_head->next,std::memory_order_acquire,std::memory_order_relaxed))
    {
        //如果head_的值没有被其他线程改变，则将old_head->next作为新的head_,因为这是pop操作
    }
    if(old_head){
        value=old_head->data;
        delete old_head;
        return true;
    }
    return false;
}


template<typename T>    
LockFreeStack<T>::~LockFreeStack()
{
    Node* current=head_.load(std::memory_order_seq_cst);
    while(current){
        Node* temp=current;
        current=current->next;
        delete temp;
    }
}

void producer(LockFreeStack<int>& stack)
{
    for(int i=0;i<100;++i){
        stack.push(i);
    }
}

void consumer(LockFreeStack<int>& stack)
{
    for(int i=0;i<100;++i){
        int value;  
        if (stack.pop(value))
        {
            std::cout << "value: " << value << std::endl;
        }   
    }
}

int main() {
    LockFreeStack<int> stack;
    std::thread t1(producer, std::ref(stack));
    std::thread t2(consumer, std::ref(stack));

    t1.join();
    t2.join();
    return 0;
}
