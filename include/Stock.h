#ifndef STOCK_H
#define STOCK_H

#include "FinancialAsset.h"
#include "PriceHistory.h"
#include <string>

using std::string;

class Stock : public FinancialAsset {
private:
    PriceHistory* history;

public:
    Stock(const string& ticker, const string& name);
    ~Stock();

    void loadFromCSV(const string& filename);
    PriceHistory* getHistory() const;
    double getPriceOnDate(const string& date) const;
    double calculateAnnualReturn(int year) const override;
    void printSummary() const override;
    string getType() const override;
};

#endif