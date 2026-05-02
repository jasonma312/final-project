#ifndef PRICENODE_H
#define PRICENODE_H

#include <string>
using std::string;

/**
 * PriceNode 
 * 
 * A node in a Doubly linked list representing a single day of stock market data. 
 * 
 * Each node corresponds to one row from the CSV file containing historical stock prices, 
 * typically with columns: 
 *      Date, Open, High, Low, Close, Adj Close, Volume
 * 
 * Stock Terminology: 
 * 
 * Open: Price of the stock at the start of the trading day. 
 * High: Highest price reached during the trading day. 
 * Low: Lowest price reached during the trading day. 
 * Close: Final trading price at market close for that day. 
 * Adj Close (Adjusted Close): The closing price adjusted for corperate actions such as 
 *  - stock splits 
 *  - dividends 
 *  - mergers
 * Volume: Number of shares traded that day. 
 * 
 * Data Structure Behavior: 
 */
struct PriceNode {
    string      date;        // "YYYY-MM-DD"
    double      open;
    double      high;
    double      low;
    double      close;
    long        volume;
    PriceNode*  next;
    PriceNode*  prev;

    PriceNode(const string& d, double o, double h, double l, double c, long v)
    :   date(d), 
        open(o), 
        high(h), 
        low(l), 
        close(c), 
        volume(v), 
        next(nullptr), 
        prev(nullptr) {}
};

#endif