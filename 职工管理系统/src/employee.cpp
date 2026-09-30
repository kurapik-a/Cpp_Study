#include "employee.h"

using namespace std;

employee::employee(int id, string name, int dId)
{
    // 这三个是父类中的公共属性
    this->id = id;
    this->name = name;
    this->deptid = dId;
}

employee::~employee()
{
}

void employee::showInfo()
{
    cout << "员工编号" << this->id
         << "\t员工姓名" << this->name
         << "\t岗位" << this->getDeptid()
         << "\t完成任务，牛马" << endl;

} // 展示员工信息
string employee::getDeptid()
{
    return string("员工");

} // 展示部门信息