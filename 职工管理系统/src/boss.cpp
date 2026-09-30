#include <iostream>
#include "boss.h"

boss::boss(int id, string name, int dId)
{
    this->id = id;
    this->name = name;
    this->deptid = dId;
}

boss::~boss()
{
}

void boss::showInfo()
{
    cout << "员工编号" << this->id
         << "\t员工姓名" << this->name
         << "\t岗位" << this->getDeptid()
         << "\t布置任务" << endl;

} // 展示员工信息
string boss::getDeptid()
{
    return string("老板");

} // 展示部门信息