#pragma once
#include <string>
#include "worker.h"

using namespace std;

class boss:public worker
{
private:
    /* data */
public:
    boss(int id, string name, int dId);
    ~boss();
    virtual void showInfo();    // 展示员工信息
    virtual string getDeptid(); // 展示部门信息
};


