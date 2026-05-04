#ifndef FIXEDSIPSTRATEGY_H
#define FIXEDSIPSTRATEGY_H

#include "TradingStrategy.h"

class FixedSIPStrategy : public TradingStrategy {
public:
    //constructor and destructor 
    FixedSIPStrategy();
    ~FixedSIPStrategy() override;

    //backtest function specific to this strategy
    //virtual function implemented into this class
    SimResult backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) override;

    string getName() const override;
};

#endif 