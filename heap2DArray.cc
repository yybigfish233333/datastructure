#include <iostream>
#include <stdexcept>

template <typename T>
class twodarrayinheap
{
private:
    int hang;
    int lie;
    T *data_;
    int index(int i, int j)
    {
        return i * hang + j;
    }

public:
    twodarrayinheap(int hang, int lie) : hang(hang), lie(lie), data_(nullptr)
    {
        data_ = new T[hang * lie];
    }
    ~twodarrayinheap()
    {
        delete[] data_;
    }

    twodarrayinheap(const twodarrayinheap &other) : hang(other.hang), lie(other.lie), data_(nullptr)
    {
        data_ = new T[hang * lie];
        for (int i = 0; i < hang * lie; i++)
        {
            data_[i] = other.data_[i];
        }
        // 拷贝构造
    }

    twodarrayinheap &operator=(const twodarrayinheap &other)
    {
        if (this == &other)
        {
            return *this; // 防止自赋值
        }
        delete[] data_;
        hang = other.hang;
        lie = other.lie;
        data_ = new T[hang * lie];
        for (int i = 0; i < hang * lie; i++)
        {
            data_[i] = other.data_[i];
        }
        return *this;
        // 拷贝赋值运算符
    }

    twodarrayinheap(twodarrayinheap &&other) noexcept : hang(other.hang), lie(other.lie), data_(other.data_)
    {
        other.data_ = nullptr;
        other.hang = other.lie = 0;
        // 移动构造函数
    }

    twodarrayinheap& operator=(twodarrayinheap&& other) noexcept{
        if (other==this)
        {
            return *this;//防止自赋值
        }
        delete[] data_;

        hang=other.hang;
        lie=other.lie;
        data_=other.data_;

        other.data_=nullptr;
        other.hang=other.lie=0;

        return *this;
        //移动赋值运算符
    }

    T& operator()(int i,int j){
        if (i>=hang||lie>=lie)
        {
            throw std::out_of_range("index out of bounds");

        }
        return data_[index(i,j)];
    }

    const T& operator()(int i,int j) const{
        if (i>=hang||lie>=lie)
        {
            throw std::out_of_range("index out of bounds");

        }
        return data_[index(i,j)];
    }
};
