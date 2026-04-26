#ifndef FINANCIALASSET_H
#define FINANCIALASSET_H

#include <string>

using std::string;

class FinancialAsset {
protected:
    string ticker;
    string name;
    string sector;

public:
    FinancialAsset(const string& ticker, const string& name);
    virtual ~FinancialAsset();

    virtual void printSummary() const = 0;
    virtual double calculateAnnualReturn(int year) const = 0;
    virtual string getType() const = 0;

    string getTicker() const;
    string getName() const;

    void setSector(const string& sector);
    string getSector() const;
};

#endif