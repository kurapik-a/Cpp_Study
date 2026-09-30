#ifndef COMPUTER_H
#define COMPUTER_H

class abstract_cpu
{
private:
    /* data */
public:
    abstract_cpu(/* args */);
    virtual ~abstract_cpu();

    virtual int calcuate() = 0;
};

class abstractgpu
{
private:
    /* data */
public:
    abstractgpu(/* args */);
    ~abstractgpu();
    virtual void videocalcuate() = 0;
};

#endif
