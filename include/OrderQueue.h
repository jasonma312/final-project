#ifndef ORDERQUEUE_H
#define ORDERQUEUE_H

#include <string>
using std::string;

//--------------------------- Order Struct ---------------------------
//encodes the details of a pending order that has not yet been executed
struct Order {
    string ticker;
    string type;    //"MARKET" or "LIMIT"
    string side;    //"BUY" or "SELL"
    double targetPrice;
    int shares;
    string submittedDate;
};

//the node contains that order and a pointer to the next node in the queue
struct QueueNode {
    Order      data;
    QueueNode* next;
};

//------------------------------ OrderQueue Class ------------------------------
class OrderQueue {
private:
    QueueNode* front;
    QueueNode* back;
    int size;

public:
    OrderQueue();
    ~OrderQueue(); 

    void enqueue(const Order& order);
    Order dequeue();
    Order peek() const;
    bool isEmpty() const;
    int  getSize() const;
    void printAll() const;  //front to back
};

#endif