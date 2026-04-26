#ifndef ETF_H
#define ETF_H

#include "Stock.h"

class ETF: public Stock {
private:
    double expenseRatio;

public:
    ETF(const string& ticker, const string& name, double expenseRatio);

    void printSummary() const override;
    string getType() const override;
    double getExpenseRatio() const;
};

#endif