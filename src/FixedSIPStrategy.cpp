#include "FixedSIPStrategy.h"
#include "CSVParser.h"
#include <vector>

using std::string;
using std::vector;

FixedSIPStrategy::FixedSIPStrategy()  {}
FixedSIPStrategy::~FixedSIPStrategy() {}

//name of the strategy, used for result printing
string FixedSIPStrategy::getName() const {
    return "Fixed SIP";
}

SimResult FixedSIPStrategy::backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) {
    SimResult result;   //store the final results of simulation in the struct
    //initialize all parameters
    result.strategyName = getName();
    result.finalValue = 0.0;
    result.totalInvested = 0.0;
    result.totalReturn = 0.0;
    result.cagr = 0.0;
    result.maxDrawdown = 0.0;
    result.totalTrades = 0;

    //no data to work off of then return basic 
    if (history == nullptr || history->getSize() == 0) return result;

    //iteration parameters to keep track of position in history
    double totalShares = 0.0;
    //keep track of dates
    int prevMonth = -1; 
    int prevYear = -1;
    double lastClose = 0.0; //last closing price

    //portfolio values at end of each month, store it in this vector and save for calcualations later
    vector<double> portfolioValues;

    //iterate foward throught the history
    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {

        //extract year and month
        PriceNode& node = *it;
        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);

        //skip data outside of our simulation range
        if (year < startYear) continue;
        if (year > endYear) break;

        //create a boolean to check if we are in a new month
        //we want to invest on the first day of each new month
        //if year has changed then we must've changed months, or if the previous month is now different from the current month
        bool newMonth = (year != prevYear || month != prevMonth);

        //if new month, we want to invest the fixed amount
        if (newMonth) {
            //update the prev parameters to the current month and year for the next iteration
            prevYear  = year;
            prevMonth = month;

            //invest the fixed amount monthly capital
            if (node.close > 0.0) {
                //calculate how many shares we can buy with the fixed monthly capital and add it to our total shares
                double shares  = monthlyCapital / node.close;
                totalShares += shares;  //increase total shares by the amount we bought with fixed capital 
                result.totalInvested += monthlyCapital; //increase total invested by the fixed monthly capital
                result.totalTrades++;   //increase total trades

                //use the vector from before to store the portfolio val at end of each month
                //use to calculate later on (max drawdown)
                portfolioValues.push_back(totalShares * node.close);
            }
        }

        //update last closing price for calculations
        lastClose = node.close;
    }

    //invalid simulation result so quit before errors in calculation
    if (totalShares <= 0.0 || lastClose <= 0.0) return result;

    //final value calcualtion (total shares * last closing price)
    result.finalValue =  totalShares * lastClose;
    double finalValue = result.finalValue;   //store in a variable for easier access in calculations

    //calculate the total return of the strategy given by (finalValue - totalInvested) / totalInvested * 100
    if (result.totalInvested > 0.0) {
        double profit = finalValue - result.totalInvested;
        result.totalReturn = (profit / result.totalInvested) * 100.0;
    } else {
        result.totalReturn = 0.0;
    }

    //use cagr helper function
    int years = endYear - startYear;
    result.cagr = calculateCAGR(result.totalInvested, finalValue, years);

    //uuse max drawdown helper function
    result.maxDrawdown = calculateMaxDrawdown(portfolioValues);
}