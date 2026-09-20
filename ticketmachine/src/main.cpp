#include <iostream>

#include "TicketMachine.h"

using namespace std;

int main()
{
    cout << "hello world. I'm " << 23 << " today" << endl;
    TicketMachine ts;
    ts.insertMoney(100);
    ts.insertMoney(300);
    ts.showBalance();

    return 0;
}