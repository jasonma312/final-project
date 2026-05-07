#include "MomentumStrategy.h"
#include "CSVParser.h"
#include <string> 

using std::string; 

MomentumStrategy::MomentumStrategy(double mt) : MinimumThreshold(mt) {}
MomentumStrategy::~MomentumStrategy() {}

SimResult MomentumStrategy::backtest(PriceHistory* history, double monthCapital, int startYear, int endYear) { 
    SimResult result; 

    return result; 
}

string MomentumStrategy::getName() const {
    return "6-Month Momentum"; 
}
