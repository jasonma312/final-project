#ifndef TRADINGSTRATEGY_H
#define TRADINGSTRATEGY_H

#include <string>
#include <vector>
#include "PriceHistory.h"

using std::string;
using std::vector;

struct SimResult {
    string strategyName;  //strat name 
    double finalValue;    //porfolio value at the end of simulation
    double totalInvested; //invested amount 
    double totalReturn;   // follows (finalValue - totalInvested) / totalInvested * 100
    double cagr;          //compound annual growth rate
    double maxDrawdown;   //worst peak-to-trough during simululation 
    int totalTrades;   //total trade (buy or sell) executed during simulation
};

class TradingStrategy {
public:
    //use virtual functions to allow for polymorphism, so we can have different strategies that implement the same interface
    //these wont be implemented in TradingStrategy.cpp, actually implemented in derived trading classes (inherited from this)
    virtual ~TradingStrategy();
    virtual SimResult backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) = 0;
    virtual string getName() const = 0;
    //result printing
    void printResult(const SimResult& result) const;

protected:
    //calculation functions helpers
    double calculateCAGR(double startVal, double endVal, int years) const;
    double calculateMaxDrawdown(const vector<double>& portfolioValues) const;
};

#endif