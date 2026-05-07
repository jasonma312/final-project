#ifndef DYNAMICSIPSTRATEGY_H
#define DYNAMICSIPSTRATEGY_H

#include "TradingStrategy.h"
class DynamicSIPStrategy : public TradingStrategy {
private:
    double baseAmount;   //base amount to invest monthly
    double increasePct;  //percentage to increase the monthly investment by when price drops
    double decreasePct;  //percentage to decrease the monthly investment by when price rises
public:
    DynamicSIPStrategy(double base, double inc, double dec);
    ~DynamicSIPStrategy();

    SimResult backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) override;
    string getName() const override;
};

#endif