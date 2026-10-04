#pragma once
#include <iostream>
using namespace std;

template <class T>
class MyArray
{
public:
    // 构造函数
    MyArray(int capacity)
    {
        this->m_Capacity = capacity;
        this->m_Size = 0;
        pAddress = new T[this->m_Capacity];
    }

    // 拷贝构造
    MyArray(const MyArray &arr)
    {
        this->m_Capacity = arr.m_Capacity;
        this->m_Size = arr.m_Size;
        this->pAddress = new T[this->m_Capacity];
        for (int i = 0; i < this->m_Size; i++)
        {
            // 如果T为对象，而且还包含指针，必须需要重载 = 操作符，因为这个等号不是 构造 而是赋值，
            //  普通类型可以直接= 但是指针类型需要深拷贝
            this->pAddress[i] = arr.pAddress[i];
        }
    }

    // 重载= 操作符  防止浅拷贝问题
    MyArray &operator=(const MyArray &myarray)
    {

        if (this->pAddress != NULL)
        {
            delete[] this->pAddress;
            this->m_Capacity = 0;
            this->m_Size = 0;
        }

        this->m_Capacity = myarray.m_Capacity;
        this->m_Size = myarray.m_Size;
        this->pAddress = new T[this->m_Capacity];
        for (int i = 0; i < this->m_Size; i++)
        {
            this->pAddress[i] = myarray[i];
        }
        return *this;
    }

    // 重载[] 操作符  arr[0]
    T &operator[](int index)
    {
        return this->pAddress[index]; // 不考虑越界，用户自己去处理
    }

    // 尾插法
    void Push_back(const T &val)
    {
        if (this->m_Capacity == this->m_Size)
        {
            return;
        }
        this->pAddress[this->m_Size] = val;
        this->m_Size++;
    }

    // 尾删法
    void Pop_back()
    {
        if (this->m_Size == 0)
        {
            return;
        }
        this->m_Size--;
    }

    // 获取数组容量
    int getCapacity()
    {
        return this->m_Capacity;
    }

    // 获取数组大小
    int getSize()
    {
        return this->m_Size;
    }

    // 析构
    ~MyArray()
    {
        if (this->pAddress != NULL)
        {
            delete[] this->pAddress;
            this->pAddress = NULL;
            this->m_Capacity = 0;
            this->m_Size = 0;
        }
    }

private:
    T *pAddress;    // 指向一个堆空间，这个空间存储真正的数据
    int m_Capacity; // 容量
    int m_Size;     // 大小
};

/*
浅拷贝
        arr (原对象)                      copy (拷贝对象)
   ┌──────────────────┐              ┌──────────────────┐
   │ m_Capacity: 5    │              │ m_Capacity: 5    │
   │ m_Size: 3        │              │ m_Size: 3        │
   │                  │              │                  │
   │ pAddress ●───────┼──────┐  ┌────┼───────● pAddress │
   └──────────────────┘      │  │    └──────────────────┘
                             ▼  ▼
                        ┌─────────────┐
                        │ 堆内存 (共享) │
                        ├─────────────┤
                        │ [0]: 10     │
                        │ [1]: 20     │
                        │ [2]: 30     │
                        │ [3]: 垃圾值 │
                        │ [4]: 垃圾值 │
                        └─────────────┘

   问题：
   1. copy 改数据 → arr 的数据也跟着变（共享同一块内存）
   2. 析构时 arr delete[] 一次，copy 又 delete[] 一次
      → 同一块内存释放两次 → 程序崩溃 💥


深拷贝
        arr (原对象)                      copy (拷贝对象)
   ┌──────────────────┐              ┌──────────────────┐
   │ m_Capacity: 5    │              │ m_Capacity: 5    │
   │ m_Size: 3        │              │ m_Size: 3        │
   │                  │              │                  │
   │ pAddress ●───────┼──────┐  ┌────┼───────● pAddress │
   └──────────────────┘      │  │    └──────────────────┘
                             ▼  ▼
                     ┌───────────┐ ┌───────────┐
                     │ 堆内存 A  │ │ 堆内存 B   │ ← new T[m_Capacity] 新开的
                     ├───────────┤ ├───────────┤
                     │ [0]: 10   │ │ [0]: 10   │ ← 逐个复制
                     │ [1]: 20   │ │ [1]: 20   │ ← pAddress[i] = arr.pAddress[i]
                     │ [2]: 30   │ │ [2]: 30   │
                     │ [3]: 垃圾 │ │ [3]: 垃圾 │
                     │ [4]: 垃圾 │ │ [4]: 垃圾 │
                     └───────────┘ └───────────┘

   ✓ 两块独立内存，互不影响
   ✓ 析构时各自 delete[] 自己的内存，安全


*/