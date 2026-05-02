#ifndef TRADESTACK_H
#define TRADESTACK_H

#include <string>
using std::string;

//--------------------------------- Trade Record & Stack Node ---------------------
//use this to encode the details of a trade
struct TradeRecord {
    string ticker;
    string date;
    double price;
    int shares;
    string action;
    double totalCost;
};

//the node contains that trade record and a pointer to the next node in the stack

//------------------------------ TradeStack Class ------------------------------
class TradeStack {
private:
    struct StackNode {
        TradeRecord data;
        StackNode*  next;
    };

    StackNode* top;
    int size;

public:
    TradeStack();
    ~TradeStack(); // frees all StackNodes

    void push(const TradeRecord& record);
    TradeRecord pop();
    TradeRecord peek() const;
    bool isEmpty() const;
    int  getSize() const;
    void printAll() const;  //top to bottom order
};

#endif