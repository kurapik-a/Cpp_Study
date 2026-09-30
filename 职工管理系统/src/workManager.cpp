#include "workManager.h"

using namespace std;

WorkManager::WorkManager(/* args */)
{
    this->m_empnum = 0;
    this->m_emparr = NULL;
}

WorkManager::~WorkManager()
{
    if (m_emparr != NULL)
    {
        for (int i = 0; i < m_empnum; i++)
            delete m_emparr[i]; // 先释放每个职工对象
        delete[] m_emparr;      // 再释放指针数组本身
        m_emparr = NULL;
    }
}

void WorkManager::saveemp()
{

    ofstream ofs;
    ofs.open(FILENAME, ios::out);
    for (int i = 0; i < this->m_empnum; i++)
    {
        ofs << this->m_emparr[i]->id << " "
            << this->m_emparr[i]->name << " "
            << this->m_emparr[i]->deptid << endl;
    }

    ofs.close();
}

void WorkManager::Show_Menu()
{
    cout << "********************************************" << endl;
    cout << "*********  欢迎使用职工管理系统！ **********" << endl;
    cout << "*************  0.退出管理程序  *************" << endl;
    cout << "*************  1.增加职工信息  *************" << endl;
    cout << "*************  2.显示职工信息  *************" << endl;
    cout << "*************  3.删除离职职工  *************" << endl;
    cout << "*************  4.修改职工信息  *************" << endl;
    cout << "*************  5.查找职工信息  *************" << endl;
    cout << "*************  6.按照编号排序  *************" << endl;
    cout << "*************  7.清空所有文档  *************" << endl;
    cout << "********************************************" << endl;
    cout << endl;
}

void WorkManager::ExitSystem()
{
    cout << "欢迎下次使用" << endl;
    // system("pause");
    exit(0);
}

void WorkManager::addemployee()
{
    cout << "请输入增加职工数量： " << endl;

    int addNum = 0;
    cin >> addNum;
    if (addNum > 0)
    {
        // 计算新总空间大小
        int newSize = m_empnum + addNum;
        // 开辟新空间
        worker **wk = new worker *[newSize];
        // 将原空间下内容存放到新空间下
        if (m_emparr != NULL)
        {
            for (int i = 0; i < m_empnum; i++)
            {
                wk[i] = m_emparr[i];
            }
        }

        // 输入新数据
        for (int i = 0; i < addNum; i++)
        {
            int id;
            string name;
            int dSelect;

            cout << "请输入第 " << i + 1 << "个员工的编号" << endl;
            cin >> id;
            cout << "请输入第 " << i + 1 << "个员工的姓名" << endl;
            cin >> name;

            cout << "请选择该职工的岗位：" << endl;
            cout << "1、普通职工" << endl;
            cout << "2、老板" << endl;
            cin >> dSelect;
            worker *wp = NULL;
            switch (dSelect)
            {
            case 1:
                wp = new employee(id, name, 1);
                break;
            case 2:
                wp = new boss(id, name, 2);
                break;

            default:
                cout << "岗位输入有误，默认按普通职工处理" << endl;
                wp = new employee(id, name, 1);
                break;
            }
            wk[m_empnum + i] = wp;
        }

        // 释放原有空间
        delete[] m_emparr;
        // 更改新空间的指向
        m_emparr = wk;
        // 更新新的个数
        m_empnum = newSize;
        // 提示信息
        cout << "成功添加" << addNum << "名新职工！" << endl;
        saveemp();
    }
    else
    {
        cout << "输入有误" << endl;
    }
}