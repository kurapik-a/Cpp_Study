#ifndef WORKMANAGER_H
#define WORKMANAGER_H

#include <iostream>

#include <fstream>
#define FILENAME "empfile.txt"

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
    bool m_fileisempty; // 标志文件是否为空

    WorkManager(/* args */);
    ~WorkManager();
    void Show_Menu();
    void ExitSystem();
    void addemployee();
    void saveemp();
    // 统计人数
    int getempnum();
    // 初始化员工
    void initemp();
    // 显示职工
    void showemp();
};

#endif
