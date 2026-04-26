#ifndef PRICEHISTORY_H
#define PRICEHISTORY_H

#include <string>
#include "PriceNode.h"

using std::string;

//this class is doubly linked list of PriceNodes with additional methods for searching/printing
class PriceHistory {
private:
    PriceNode* head;
    PriceNode* tail;
    int size;

public:
    //--------------------- Constructors & Destructors --------------------
    PriceHistory();
    ~PriceHistory();

    void append(const string& date, double open, double high, double low, double close, long volume);
    PriceNode* findByDate(const string& date) const;
    void printRange(const string& startDate, const string& endDate) const;

    int getSize() const;

    //-------------------- Iterators --------------------
    class Iterator {
    private:
        PriceNode* current;

    public:
        Iterator(PriceNode* node);
        Iterator& operator++();
        PriceNode& operator*();
        bool operator!=(const Iterator& other) const;
    };

    Iterator begin() const;
    Iterator end() const;

    class ReverseIterator {
    private:
        PriceNode* current;

    public:
        ReverseIterator(PriceNode* node);
        ReverseIterator& operator++();
        PriceNode& operator*();
        bool operator!=(const ReverseIterator& other) const;
    };

    ReverseIterator rbegin() const;
    ReverseIterator rend() const;
};

#endif