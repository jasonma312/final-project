#ifndef MOMENTUMSTRATEGY_H
#define MOMENTUMSTRATEGY_H

#include "TradingStrategy.h"

class MomentumStrategy : public TradingStrategy { 
public: 
    MomentumStrategy(); 
    ~MomentumStrategy(); 

    SimResult backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) override; 

    string getName() const override; 
};


#endif MOMENTUMSTRATEGY_H