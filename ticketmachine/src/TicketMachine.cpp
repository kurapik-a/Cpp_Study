#include "TicketMachine.h"

#include <iostream>
using namespace std;



void TicketMachine::showPrompt()
{
    cout << "something";
}

void TicketMachine::insertMoney(int money)
{

    balance += money;
}

void TicketMachine::showBalance()
{
    cout << "balance: " << balance << endl;
}

void TicketMachine::printTicket()
{
    cout << "ticket: " << PRICE << endl;
}

void TicketMachine::showTotal()
{
    cout << "total:" << endl;
}