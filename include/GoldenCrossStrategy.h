#ifndef GOLDENCROSSSTRATEGY_H
#define GOLDENCROSSSTRATEGY_H

#include "TradingStrategy.h"
#include "CircularQueue.h"

//golden cross strategy: buy when 50-day moving average crosses above 200-day moving average sell when it crosses below
class GoldenCrossStrategy : public TradingStrategy {
private:
    //helper to determine crossover events
    int crossover(double shortMA, double longMA, bool prevShortAboveLong) const;

public:
    //constructor destructor
    GoldenCrossStrategy();
    ~GoldenCrossStrategy() override;

    //backtest implementation for this class instance
    SimResult backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) override;

    string getName() const override; //returns "Golden Cross Strategy"
};

#endif