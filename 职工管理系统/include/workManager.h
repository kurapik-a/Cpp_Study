#ifndef WORKMANAGER_H
#define WORKMANAGER_H

#include <iostream>
#include "worker.h"
#include "employee.h"
#include "boss.h"

using namespace std;

class WorkManager
{
private:
    /* data */
public:
    int m_empnum; // 记录文件中的人数个数
    worker **m_emparr; // 员工数组的指针

    WorkManager(/* args */);
    ~WorkManager();
    void Show_Menu();
    void ExitSystem();
    void addemployee();
};

#endif
