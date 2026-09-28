#include <iostream>
#include "min.h"

using namespace std;

class person
{
private:
    /* data */

public:
    int age;

    person(/* args */ int age)
    {
        // this指向p1。
        this->age = age;
    }

    ~person()
    {
    }
    //引用相当于给变量起别名
    person& addage(person &p)
    {
        this->age += p.age;
        return *this;
    }
};

void test01()
{
    person p1(10);
    cout << "p1的年龄是：" << p1.age << endl;
}

void test02()
{
    person p1(10);
    person p2(10);

    p2.addage(p1).addage(p1);
    cout << "p2的年龄是：" << p2.age << endl;
}

int main()
{
    test02();

    return 0;
}
