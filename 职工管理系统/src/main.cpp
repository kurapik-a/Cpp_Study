#include <iostream>
#include "workManager.h"
#include <string.h>

#include "employee.h"
#include "boss.h"
   
using namespace std;

void test01()
{
    employee employee1(1, "张三", 1);
    employee1.showInfo();
    employee employee2(2, "李三", 2);
    employee2.showInfo();
}

void test02()
{
    // 多态体现，同一个指针->不同的结果
    worker *worker = new employee(1, "张三", 1);
    worker->showInfo();
    delete worker;

    worker = new employee(2, "李三", 2);
    worker->showInfo();
    delete worker;

    worker = new boss(1, "big Boss", 1);
    worker->showInfo();
    delete worker;

    worker = NULL;
}

int main()
{
    WorkManager wm;

    int choice = 0;

    while (true)
    {
        wm.Show_Menu();
        cout << "请输入你的选择" << endl;
        cin >> choice;

        switch (choice)
        {
        case 0:
            // 退出
            wm.ExitSystem();
            break;
        case 1:
            // 增加
            wm.addemployee();
            break;
        case 2:
            // 显示
            wm.showemp();
            break;
        case 3:
            // 删除
            wm.Delemp();
            break;
        case 4:
            // 修改
            wm.Modemp();
            break;
        case 5:
            // 查找
            wm.Findemp();
            break;
        case 6:
            // 排序
            wm.Sortemp();
            break;
        case 7:
            // 清空

            break;

        default:
            break;
        }
    }

    return 0;
}