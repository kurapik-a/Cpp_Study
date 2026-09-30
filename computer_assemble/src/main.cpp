#include <iostream>
#include "computer.h"
#include <string.h>

using namespace std;

class Intelcpu : public abstract_cpu
{
private:
    /* data */
    string C_name;
    string C_Hz;

public:
    Intelcpu(string name, string hz);
    ~Intelcpu();
    int calcuate()
    {
        cout << C_name << "的频率是" << C_Hz << endl;
        return 0;
    }
};

Intelcpu::Intelcpu(string name, string hz) : C_name(name), C_Hz(hz)
{
}

Intelcpu::~Intelcpu()
{
}

class nvideagpu : public abstractgpu
{
private:
    string rongliang;
    string mingzi;

public:
    nvideagpu(string name, string gb);
    ~nvideagpu();
    virtual void videocalcuate()
    {
        cout << mingzi << "的内存为" << rongliang << endl;
    }
};

nvideagpu::nvideagpu(string name, string gb)
{
    mingzi = name;
    rongliang = gb;
}

nvideagpu::~nvideagpu()
{
}

class computer
{
private:
    /* data */
    abstract_cpu *cpu; // 成员持有 CPU 指针
    abstractgpu *gpu;

public:
    computer(abstract_cpu *c, abstractgpu *g);
    ~computer();
    void dowork()
    {
        cpu->calcuate();
        gpu->videocalcuate();
    }
};

computer::computer(abstract_cpu *c, abstractgpu *g) : cpu(c),gpu(g)
{
}

computer::~computer()
{
    delete cpu; // 基类析构是虚函数，会正确调用 ~Intelcpu()
    cpu = NULL; // 防止悬空指针（好习惯）
    delete gpu;
    cpu = NULL; // 防止悬空指针（好习惯）
}

void test01()
{
    computer c(new Intelcpu("i5-12400", "4.4GHz"),new nvideagpu("5060ti","16g"));
    c.dowork();
} // c 离开作用域自动调用 ~computer()，内部 delete 掉 Intelcpu
int main()
{
    test01();
    return 0;
}