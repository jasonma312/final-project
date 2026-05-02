#include "TradeStack.h"
#include <iostream>
#include <stdexcept>

TradeStack::TradeStack() : top(nullptr), size(0) {}

TradeStack::~TradeStack() {
    while(!isEmpty()){ 
        pop(); 
    }
}

void TradeStack::push(const TradeRecord& record) {
    StackNode* newNode = new StackNode{record, top}; 
    top = newNode; 
    size++; 
}

TradeRecord TradeStack::pop() { 
    if(isEmpty()) { 
        throw std::runtime_error("Stack is empty");
    }
    StackNode* temp = top; 
    TradeRecord record = top->data; 

    top = top->next; 
    delete temp; 
    size--; 

    return top->data; 
}

TradeRecord TradeStack::peek() const { 
    if(isEmpty()) {
        throw std::runtime_error("Stack is empty"); 
    }
    return top->data; 
}

bool TradeStack::isEmpty() const { 
    return top == nullptr; 
}

int TradeStack::getSize() const { 
    return size;
}

void TradeStack::printAll() const { 
    StackNode* current = top; 

    while(current != nullptr) { 
        const TradeRecord& r = current->data; 
        std::cout   << "Ticker: " << r.ticker 
                    << ", Date: " << r.date
                    << ", Price: " << r.price
                    << ", Shares: " << r.shares
                    << ", Action: " << r.action
                    << ", Total Cost: " << r.totalCost 
                    << std::endl;
        current = current->next; 
    }
}