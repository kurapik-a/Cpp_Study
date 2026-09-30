#pragma once

#include <iostream>
#include "worker.h"
using namespace std;

class employee : public worker
{
private:
    

public:
    employee(int id,string name,int dId);
    ~employee();
    virtual void showInfo();    // 展示员工信息
    virtual string getDeptid(); // 展示部门信息
};


