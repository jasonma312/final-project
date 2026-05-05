#include "GoldenCrossStrategy.h"
#include "CSVParser.h"
#include <vector>

using std::string;
using std::vector;

//constructor destructor
GoldenCrossStrategy::GoldenCrossStrategy()  {}
GoldenCrossStrategy::~GoldenCrossStrategy() {}

//get name of the strategy
string GoldenCrossStrategy::getName() const {
    return "Golden Cross";
}

//helper function to determine crossover events
int GoldenCrossStrategy::crossover(double shortMA, double longMA, bool prevShortAboveLong) const {
    bool nowAbove = (shortMA > longMA); //50 day MA greater than 200 day MA?
    if (!prevShortAboveLong && nowAbove)  return  1; //buy inidication 
    if (prevShortAboveLong && !nowAbove) return -1; //sell indication
    return 0;
}

SimResult GoldenCrossStrategy::backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) {
    //initialize simulation result
    SimResult result;
    result.strategyName = getName();
    result.finalValue = 0.0;
    result.totalInvested = 0.0;
    result.totalReturn = 0.0;
    result.cagr = 0.0;
    result.maxDrawdown = 0.0;
    result.totalTrades = 0;

    //no data case
    if (history == nullptr || history->getSize() == 0) return result;

    //use two circular queues to track moving averages (50 day and 200 day)
    CircularQueue shortCQ(50);
    CircularQueue longCQ(200);

    //portfolio variables for position in simulation
    double cashBalance = 0.0;
    double totalShares = 0.0;
    bool isInvested = false;
    bool prevShortAbove = false;
    int prevYear = -1;
    double lastClose = 0.0;

    //monthly portfolio values for max drawdown calculation
    vector<double> portfolioValues;

    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        //iterate through price history, feeding close prices into CQs and checking for crossovers
        PriceNode& node = *it;
        int year  = CSVParser::extractYear(node.date);

        //ensure that the simulation strategy has enough data to even calculate the moving averages
        if (year < startYear) {
            shortCQ.enqueue(node.close);
            longCQ.enqueue(node.close);
            //calculate the moving averages once we actually have enough data and establish initial relationship between the CQs
            if (longCQ.isFull()) prevShortAbove = (shortCQ.getAverage() > longCQ.getAverage());
            continue;   //no trading yet, since we just started and needed data to begin
        }
        if (year > endYear) break;

        //fix annual capital injection at the start of each year (before trading for that year begins)
        //do this because we buy off signals, not timing
        if (year != prevYear) {
            cashBalance += monthlyCapital * 12.0;
            result.totalInvested += monthlyCapital * 12.0;
            prevYear = year;
        }

        //current close price feeds into both CQs for MA calculations and crossover detection
        shortCQ.enqueue(node.close);
        longCQ.enqueue(node.close);

        //need to fill long CQ before we have MAs to check for crossover events and make trading decisions
        if (!longCQ.isFull()) {
            lastClose = node.close;
            continue;
        }

        //calculate the moving averages for current
        double shortMA = shortCQ.getAverage();
        double longMA  = longCQ.getAverage();

        //check for crosover events and trade accordingly to the results
        int signal = crossover(shortMA, longMA, prevShortAbove);
        prevShortAbove = (shortMA > longMA);

        if (signal == 1 && !isInvested && cashBalance > 0.0) {
            //buy indication and invest
            double shares = cashBalance / node.close;   //buy all
            totalShares += shares;  //total shares owned
            cashBalance = 0.0; //invested all cash
            isInvested = true; 
            result.totalTrades++; //total trades executed

        } else if (signal == -1 && isInvested && totalShares > 0.0) {
            //sell indication and liquidate position
            cashBalance += totalShares * node.close;    //sell all shares
            totalShares = 0.0;  //no shares
            isInvested = false; //not in position anymore
            result.totalTrades++;   //total trades executed
        }

        //record portfolio value at the end of each month for max drawdown calculation
        portfolioValues.push_back(cashBalance + totalShares * node.close);

        lastClose = node.close;
    }

    //final portfolio value
    result.finalValue = cashBalance + totalShares * lastClose;
    double finalValue = result.finalValue;

    //calcuate total return
    if (result.totalInvested > 0.0) {
        double profit = finalValue - result.totalInvested;
        double returnRatio = profit / result.totalInvested;
        result.totalReturn = returnRatio * 100.0;
    } else {
        result.totalReturn = 0.0;
    }

    //calculate CAGR
    int years = endYear - startYear + 1;  // safer for single-year cases
    result.cagr = calculateCAGR(result.totalInvested, finalValue, years);

    //calculate max drawdown
    result.maxDrawdown = calculateMaxDrawdown(portfolioValues);

    return result;  //simulation result
}