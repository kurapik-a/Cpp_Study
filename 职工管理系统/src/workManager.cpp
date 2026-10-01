#include "workManager.h"

using namespace std;

WorkManager::WorkManager(/* args */)
{
    ifstream ifs;
    ifs.open(FILENAME, ios::in);

    // 文件不存在情况
    if (!ifs.is_open())
    {
        this->m_empnum = 0;         // 初始化人数
        this->m_fileisempty = true; // 初始化文件为空标志
        this->m_emparr = NULL;      // 初始化数组
        ifs.close();                // 关闭文件
        return;
    }

    // 文件存在，并且没有记录
    char ch;
    ifs >> ch;
    if (ifs.eof())
    {
        this->m_empnum = 0;
        this->m_fileisempty = true;
        this->m_emparr = NULL;
        ifs.close();
        return;
    }

    // 文件存在，并且保存职工数据
    int num = this->getempnum();
    this->m_empnum = num;        // 更新成员属性
    this->m_fileisempty = false; // 更新职工不为空标志

    // 根据职工数创建数组
    this->m_emparr = new worker *[this->m_empnum];
    // 初始化职工
    initemp();
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

// 统计人数
int WorkManager::getempnum()
{
    ifstream ifs;
    ifs.open(FILENAME, ios::in);

    int id;
    string name;
    int dId;

    int num = 0;

    while (ifs >> id && ifs >> name && ifs >> dId)
    {
        // 记录人数
        num++;
    }
    ifs.close();

    return num;
}

// 初始化员工
void WorkManager::initemp()
{
    ifstream ifs;
    ifs.open(FILENAME, ios::in);

    int id;
    string name;
    int dId;

    int index = 0;
    while (ifs >> id && ifs >> name && ifs >> dId)
    {
        worker *wp = NULL;
        // 根据不同的部门Id创建不同对象
        if (dId == 1) // 1普通员工
        {
            wp = new employee(id, name, dId);
        }
        else // 老板
        {
            wp = new boss(id, name, dId);
        }
        // 存放在数组中
        this->m_emparr[index] = wp;
        index++;
    }
    ifs.close();
}

// 显示职工
void WorkManager::showemp()
{
    if (this->m_fileisempty)
    {
        cout << "文件不存在或记录为空！" << endl;
    }
    else
    {
        for (int i = 0; i < this->m_empnum; i++)
        {
            // 利用多态调用接口
            this->m_emparr[i]->showInfo();
        }
    }
}

// 删除职工
void WorkManager::Delemp()
{
    if (this->m_fileisempty)
    {
        cout << "文件不存在或记录为空！" << endl;
    }
    else
    {
        // 按职工编号删除
        cout << "请输入想要删除的职工号：" << endl;
        int id = 0;
        cin >> id;

        int index = this->Isexist(id);

        if (index != -1) // 说明index上位置数据需要删除
        {
            delete this->m_emparr[index]; // 释放该职工对象
            for (int i = index; i < this->m_empnum - 1; i++)
            {
                this->m_emparr[i] = this->m_emparr[i + 1]; // 后面的数据前移
            }
            this->m_empnum--; // 更新人数
            if (this->m_empnum == 0)
            {
                this->m_fileisempty = true; // 删空后更新文件为空标志
            }

            this->saveemp(); // 删除后数据同步到文件中
            cout << "删除成功！" << endl;
        }
        else
        {
            cout << "删除失败，未找到该职工" << endl;
        }
    }
}

int WorkManager::Isexist(int id)
{
    int index = -1;

    for (int i = 0; i < this->m_empnum; i++)
    {
        if (this->m_emparr[i]->id == id)
        {
            index = i;

            break;
        }
    }

    return index;
}

// 修改职工
void WorkManager::Modemp()
{
    if (this->m_fileisempty)
    {
        cout << "文件不存在或记录为空！" << endl;
    }
    else
    {
        cout << "请输入修改职工的编号：" << endl;
        int id;
        cin >> id;

        int ret = this->Isexist(id);
        if (ret != -1)
        {
            // 查找到编号的职工，先释放旧对象
            delete this->m_emparr[ret];

            int newId = 0;
            string newName = "";
            int dSelect = 0;

            cout << "查到： " << id << "号职工，请输入新职工号： " << endl;
            cin >> newId;

            cout << "请输入新姓名： " << endl;
            cin >> newName;

            cout << "请输入岗位： " << endl;
            cout << "1、普通职工" << endl;
            cout << "2、老板" << endl;
            cin >> dSelect;

            worker *wp = NULL;
            switch (dSelect)
            {
            case 1:
                wp = new employee(newId, newName, dSelect);
                break;
            case 2:
                wp = new boss(newId, newName, dSelect);
                break;

            default:
                cout << "岗位输入有误，默认按普通职工处理" << endl;
                wp = new employee(newId, newName, 1);
                break;
            }

            // 更改数据到数组中
            this->m_emparr[ret] = wp;

            cout << "修改成功！" << endl;

            // 保存到文件中
            this->saveemp();
        }
        else
        {
            cout << "修改失败，查无此人" << endl;
        }
    }
}

// 查找职工
void WorkManager::Findemp()
{
    if (this->m_fileisempty)
    {
        cout << "文件不存在或记录为空！" << endl;
    }
    else
    {
        cout << "请输入查找的方式：" << endl;
        cout << "1、按职工编号查找" << endl;
        cout << "2、按姓名查找" << endl;

        int select = 0;
        cin >> select;

        if (select == 1) // 按职工号查找
        {
            int id;
            cout << "请输入查找的职工编号：" << endl;
            cin >> id;

            int ret = this->Isexist(id);
            if (ret != -1)
            {
                cout << "查找成功！该职工信息如下：" << endl;
                this->m_emparr[ret]->showInfo();
            }
            else
            {
                cout << "查找失败，查无此人" << endl;
            }
        }
        else if (select == 2) // 按姓名查找
        {
            string name;
            cout << "请输入查找的姓名：" << endl;
            cin >> name;

            bool flag = false; // 查找到的标志
            for (int i = 0; i < this->m_empnum; i++)
            {
                if (this->m_emparr[i]->name == name)
                {
                    cout << "查找成功,职工编号为："
                         << this->m_emparr[i]->id
                         << " 号的信息如下：" << endl;

                    flag = true;

                    this->m_emparr[i]->showInfo();
                }
            }
            if (flag == false)
            {
                // 查无此人
                cout << "查找失败，查无此人" << endl;
            }
        }
        else
        {
            cout << "输入选项有误" << endl;
        }
    }
}

// 排序职工
void WorkManager::Sortemp()
{
    if (this->m_fileisempty)
    {
        cout << "文件不存在或记录为空！" << endl;
    }
    else
    {
        cout << "请选择排序方式： " << endl;
        cout << "1、按职工号进行升序" << endl;
        cout << "2、按职工号进行降序" << endl;

        int select = 0;
        cin >> select;

        for (int i = 0; i < this->m_empnum; i++)
        {
            int minOrMax = i;
            for (int j = i + 1; j < this->m_empnum; j++)
            {
                if (select == 1) // 升序
                {
                    if (this->m_emparr[minOrMax]->id > this->m_emparr[j]->id)
                    {
                        minOrMax = j;
                    }
                }
                else // 降序
                {
                    if (this->m_emparr[minOrMax]->id < this->m_emparr[j]->id)
                    {
                        minOrMax = j;
                    }
                }
            }

            if (i != minOrMax)
            {
                // 交换两个指针，对象本身不动
                worker *temp = this->m_emparr[i];
                this->m_emparr[i] = this->m_emparr[minOrMax];
                this->m_emparr[minOrMax] = temp;
            }
        }

        cout << "排序成功,排序后结果为：" << endl;
        this->saveemp();  // 排序后同步到文件
        this->showemp();  // 展示排序结果
    }
}

// 清空文件
void WorkManager::Cleanfile()
{
    cout << "确认清空？" << endl;
    cout << "1、确认" << endl;
    cout << "2、返回" << endl;

    int select = 0;
    cin >> select;

    if (select == 1)
    {
        // 打开模式 ios::trunc 如果存在删除文件并重新创建
        ofstream ofs(FILENAME, ios::trunc);
        ofs.close();

        if (this->m_emparr != NULL)
        {
            for (int i = 0; i < this->m_empnum; i++)
            {
                if (this->m_emparr[i] != NULL)
                {
                    delete this->m_emparr[i]; // 释放每个职工对象
                }
            }
            this->m_empnum = 0;
            delete[] this->m_emparr; // 释放指针数组本身
            this->m_emparr = NULL;
            this->m_fileisempty = true;
        }
        cout << "清空成功！" << endl;
    }
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
        // 更新职工不为空标志
        m_fileisempty = false;
        // 提示信息
        cout << "成功添加" << addNum << "名新职工！" << endl;
        saveemp();
    }
    else
    {
        cout << "输入有误" << endl;
    }
}