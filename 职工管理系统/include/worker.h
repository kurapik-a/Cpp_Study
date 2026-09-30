#pragma once

#include <iostream>
#include <string.h>
using namespace std;

class worker
{
private:
public:
    int id;
    string name;
    int deptid;
    virtual void showInfo() = 0;    // 展示员工信息
    virtual string getDeptid() = 0; // 展示部门信息

    worker(/* args */)
    {
    }
    virtual ~worker()
    {
    }
};
