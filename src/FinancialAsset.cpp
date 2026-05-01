#include "FinancialAsset.h"
#include <string>

using namespace std;

FinancialAsset::FinancialAsset(const string& ticker, const string& name)
    : ticker(ticker), name(name), sector("Unknown") {}

FinancialAsset::~FinancialAsset() {}

//virtuals are then defined in the classes that use this class

//simply extract data from the class
string FinancialAsset::getTicker() const { return ticker; }
string FinancialAsset::getName()   const { return name;   }
string FinancialAsset::getSector() const { return sector; }

void FinancialAsset::setSector(const string& s) { sector = s; }