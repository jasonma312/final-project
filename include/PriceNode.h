#ifndef PRICENODE_H
#define PRICENODE_H

#include <string>
using std::string;

struct PriceNode {
    string date;
    double open;
    double high;
    double low;
    double close;
    long volume;
    PriceNode* next;
    PriceNode* prev;

    //with additional constructor for easier node creation
    PriceNode(const string& d, double o, double h, double l, double c, long v)
            : date(d), open(o), high(h), low(l), close(c), volume(v), next(nullptr), prev(nullptr) {}
};

#endif