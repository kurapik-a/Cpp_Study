#ifndef TICNETMACHINE_H
#define TICNETMACHINE_H

class TicketMachine
{
private:
    /* data */
    const int PRICE;
    int balance;
    int total;

public:
    TicketMachine(/* args */);
    ~TicketMachine();
    void showPrompt();
    void insertMoney();
    void showBalance();
    void printTicket();
    void showTotal();
};


#endif // 