#include "PriceHistory.h"
#include <iostream>
#include <iomanip>

using std::string;
using std::cout;

//constructor
PriceHistory::PriceHistory() : head(nullptr), tail(nullptr), size(0) {}

//destructor
PriceHistory::~PriceHistory() {
    //walk the nodes and delete them all, and then also the head and tail at the end. 
    PriceNode* current = head;
    while (current != nullptr) {
        PriceNode* next = current->next;
        delete current;
        current = next;
    }
    head = nullptr;
    tail = nullptr;
    size = 0;
}

//append a new entry into the price history
void PriceHistory::append(const string& date, double open, double high, double low, double close, long volume) {
    //create the new node to add
    PriceNode* node = new PriceNode(date, open, high, low, close, volume);

    //handle case of no entries, else fix the pointers to add the new node into the list
    if (tail == nullptr) {
        head = node;
        tail = node;
    } else {
        node->prev = tail;  //former tail is now the previous to this new appended node
        tail->next = node;  //former tail now points to the new node
        tail = node;        //this appended node is now the tail
    }
    size++;                 //increase the list size
}

PriceNode* PriceHistory::findByDate(const string& date) const {
    PriceNode* current = head;  //temp node

    //walk thru the list to find that node
    while (current != nullptr) {
        //keep walking to end of list, if the data matches then return
        if (current->date == date) return current;
        //otherwise keep walking
        current = current->next;
    }

    return nullptr;  //if the node isn't found, then return nullptr since the while terminates anyways
}

void PriceHistory::printRange(const string& startDate, const string& endDate) const {
    if (head == nullptr) {
        cout << "  [PriceHistory] No data available.\n";
        return;
    }

    cout << std::left
         << std::setw(12) << "Date"
         << std::setw(10) << "Open"
         << std::setw(10) << "High"
         << std::setw(10) << "Low"
         << std::setw(10) << "Close"
         << std::setw(14) << "Volume"
         << "\n";
    cout << string(66, '-') << "\n";

    int printed = 0;
    PriceNode* current = head;
    while (current != nullptr) {
        //YYYY-MM-DD
        if (current->date >= startDate && current->date <= endDate) {
            cout << std::left
                 << std::setw(12) << current->date
                 << std::fixed << std::setprecision(2)
                 << std::setw(10) << current->open
                 << std::setw(10) << current->high
                 << std::setw(10) << current->low
                 << std::setw(10) << current->close
                 << std::setw(14) << current->volume
                 << "\n";
            printed++;
        }
        if (current->date > endDate) break;
        current = current->next;
    }

    if (printed == 0) {
        cout << "  No trading days found in range " << startDate << " to " << endDate << "\n";
    } else {
        cout << "  " << printed << " trading day(s) displayed.\n";
    }
}

int PriceHistory::getSize() const {
    return size;
}

//forward iterator implementaton
//constructor
PriceHistory::Iterator::Iterator(PriceNode* node) : current(node) {}

//operator overload for ++
PriceHistory::Iterator& PriceHistory::Iterator::operator++() {
    if (current != nullptr) current = current->next;
    return *this;
}

//dereference operator overload
PriceNode& PriceHistory::Iterator::operator*() {
    return *current;
}

//not equal to conditional operator overload
bool PriceHistory::Iterator::operator!=(const Iterator& other) const {
    return current != other.current;
}

//iterator begins at head
PriceHistory::Iterator PriceHistory::begin() const {
    return Iterator(head);
}

//end is past the tail node
PriceHistory::Iterator PriceHistory::end() const {
    return Iterator(nullptr);  
}

//reverse iterator implementation
//constructor
PriceHistory::ReverseIterator::ReverseIterator(PriceNode* node) : current(node) {}

//operator overloads follow the same idea as the forward iterator counterpart
PriceHistory::ReverseIterator& PriceHistory::ReverseIterator::operator++() {
    if (current != nullptr) current = current->prev;
    return *this;
}

PriceNode& PriceHistory::ReverseIterator::operator*() {
    return *current;
}

bool PriceHistory::ReverseIterator::operator!=(const ReverseIterator& other) const {
    return current != other.current;
}

//again, same idea as the forward, but 
PriceHistory::ReverseIterator PriceHistory::rbegin() const {
    return ReverseIterator(tail);
}

PriceHistory::ReverseIterator PriceHistory::rend() const {
    return ReverseIterator(nullptr);
}