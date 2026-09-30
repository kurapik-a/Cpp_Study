#include <iostream>
#include "workManager.h"
#include <string.h>
#include "employee.h"

using namespace std;

void test01()
{
    employee employee1(1, "张三", 1);
    employee1.showInfo();
}

void test02()
{
    //多态体现，同一个指针->不同的结果
    worker *worker1 = new employee(1, "张三", 1);
    worker1->showInfo();
    worker1 = new employee(2, "李三", 2);
    worker1->showInfo();
}

int main()
{
    WorkManager wm;
    wm.Show_Menu();

    int choice = 0;
    test02();
    /*
    while (true)
    {
        wm.Show_Menu();
        cout << "请输入你的选择" << endl;
        cin >> choice;

        switch (choice)
        {
        case 0:
            //退出
            wm.ExitSystem();
            break;
        case 1:
            // 增加0

            break;
        case 2:
            // 显示

            break;
        case 3:
            // 删除

            break;
        case 4:
            // 修改

            break;
        case 5:
            // 查找

            break;
        case 6:
            // 排序

            break;
        case 7:
            // 清空

            break;

        default:
            break;
        }
    }
        */

    return 0;
}