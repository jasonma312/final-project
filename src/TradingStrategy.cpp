#include "TradingStrategy.h"
#include <iostream>
#include <iomanip>
#include <cmath>    //for powers

using std::cout;
using std::vector;

//destructor
TradingStrategy::~TradingStrategy() {}

//virutal functions are implemented by the derived classes that inherit from this base class
//no implementation for them

void TradingStrategy::printResult(const SimResult& result) const {
    cout << "\n----- Using " << result.strategyName << " -----\n";
    cout << std::fixed << std::setprecision(2); //format numbers to 2 decimal places
    //extract and print the results of the simulation that is formatted
    cout << "  Final Value   : $" << result.finalValue << "\n";
    cout << "  Total Invested: $" << result.totalInvested << "\n";
    cout << "  Total Return  :  " << result.totalReturn << "%\n";
    cout << "  CAGR          :  " << result.cagr << "%\n";
    cout << "  Max Drawdown  :  " << result.maxDrawdown << "%\n";
    cout << "  Total Trades  :  " << result.totalTrades << "\n";
}

double TradingStrategy::calculateCAGR(double startVal, double endVal, int years) const {
    if (startVal <= 0.0 || years <= 0) return 0.0;  //avoid division by zero or negative growth
    return (pow(endVal / startVal, 1.0 / static_cast<double>(years)) - 1.0) * 100.0;   //use cagr formula
}

double TradingStrategy::calculateMaxDrawdown(const vector<double>& values) const {
    //drawdown is the percentage decline from a historical peak in portfolio value to a trough before a new peak is achieved
    if (values.size() < 2) return 0.0;  //need at least 2 values to calculate drawdown

    //store values, and drawdown between them calculated as (peak - trough) / peak * 100
    double peak = values[0];
    double maxDrawdown = 0.0;

    //iterate through the portfolio values during simulation, whenever this is called, and find the greatest draw down
    for (int i = 1; i < static_cast<int>(values.size()); i++) {
        if (values[i] > peak) {
            peak = values[i];   //if we sense a new high, we update the peak value
        } else if (peak > 0.0) {    //keep calculating drawdown based on our current peaks, and update once we find the worst drawdown
            double dd = (peak - values[i]) / peak * 100.0;
            if (dd > maxDrawdown) maxDrawdown = dd;
        }
    }
    
    //return the worst drawdown recorded in the simulatin up to the point it was called 
    return maxDrawdown;
}