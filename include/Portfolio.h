#ifndef PORTFOLIO_H
#define PORTFOLIO_H

#include <string>
#include <vector>
#include "OrderQueue.h"
#include "TradeStack.h"

using std::string;
using std::vector;

//---------------------------- Position Struct ---------------------------
//encodes the details of a current holding in the portfolio
struct Position {
    string ticker;
    int shares;
    double avgCostBasis;
    double currentPrice;
};

//---------------------------- Portfolio Class ---------------------------
//manages current holdings, pending orders, and trade history
class Portfolio {
private:
    vector<Position> holdings;
    OrderQueue pendingOrders;
    TradeStack tradeHistory;
    double cashBalance;

    int findPosition(const string& ticker) const;
    void updateAvgCost(Position& pos, int newShares, double newPrice);

public:
    Portfolio(double initialCash = 0.0);
    ~Portfolio();

    void buyShares(const string& ticker, int shares, double price, const string& date);
    void sellShares(const string& ticker, int shares, double price, const string& date);

    void undoLastTrade();
    void queueOrder(const Order& order);

    void executeNextOrder(double currentPrice);

    void refreshPrice(const string& ticker, double price);
    double getTotalValue() const;
    double getTotalReturn() const;
    double getCashBalance() const;

    void printHoldings() const;
    void printOrders() const;
    void printTrades() const;

    void sortHoldingsByReturn();
    void sortHoldingsByTicker();
};

#endif