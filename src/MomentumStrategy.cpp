#include "MomentumStrategy.h"
#include "CSVParser.h"

#include <vector>
#include <cmath>
#include <algorithm>

using std::string;
using std::pow;

//constructor destructor
MomentumStrategy::MomentumStrategy(double mt)
    : momentumThreshold(mt) {}

MomentumStrategy::~MomentumStrategy() {}

//get name of the strategy
string MomentumStrategy::getName() const {
    return "Momentum";
}

SimResult MomentumStrategy::backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) {
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
    if (history == nullptr || history->getSize() == 0)
        return result;

    //portfolio variables
    double cashBalance = 0.0;
    double totalShares = 0.0;

    //track deposits by month
    int prevYear = -1;
    int prevMonth = -1;

    //momentum tracking
    double finalClose = 0.0;

    //store previous closes for lookback momentum
    std::vector<double> recentCloses;

    //portfolio values for drawdown
    std::vector<double> portfolioValues;

    //momentum lookback window
    const int LOOKBACK_DAYS = 20;

    for (PriceHistory::Iterator it = history->begin();
         it != history->end();
         ++it) {

        PriceNode& node = *it;

        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);

        //skip early data
        if (year < startYear)
            continue;

        //stop after end year
        if (year > endYear)
            break;

        //MONTHLY deposit logic
        if (year != prevYear || month != prevMonth) {

            cashBalance += monthlyCapital;

            result.totalInvested += monthlyCapital;

            prevYear = year;
            prevMonth = month;
        }

        //add close to history
        recentCloses.push_back(node.close);

        //need enough data for momentum
        if (recentCloses.size() <= LOOKBACK_DAYS) {

            finalClose = node.close;

            double portfolioValue =
                cashBalance + (totalShares * node.close);

            portfolioValues.push_back(portfolioValue);

            continue;
        }

        //calculate N-day momentum
        double oldPrice =
            recentCloses[recentCloses.size() - 1 - LOOKBACK_DAYS];

        double momentumPct =
            ((node.close - oldPrice) / oldPrice) * 100.0;

        //BUY SIGNAL
        if (momentumPct > momentumThreshold &&
            cashBalance > 0.0) {

            //invest all available cash
            double sharesToBuy =
                cashBalance / node.close;

            totalShares += sharesToBuy;

            cashBalance = 0.0;

            result.totalTrades++;
        }

        //SELL SIGNAL
        else if (momentumPct < -momentumThreshold &&
                 totalShares > 0.0) {

            //sell entire position
            cashBalance += totalShares * node.close;

            totalShares = 0.0;

            result.totalTrades++;
        }

        //track portfolio value
        double portfolioValue =
            cashBalance + (totalShares * node.close);

        portfolioValues.push_back(portfolioValue);

        finalClose = node.close;
    }

    //final portfolio value
    result.finalValue =
        cashBalance + (totalShares * finalClose);

    //total return
    result.totalReturn =
        result.finalValue - result.totalInvested;

    //CAGR
    int years = endYear - startYear + 1;

    if (result.totalInvested > 0.0 &&
        years > 0 &&
        result.finalValue > 0.0) {

        result.cagr =
            (pow(result.finalValue / result.totalInvested,
                 1.0 / years) - 1.0) * 100.0;
    }

    //MAX DRAWDOWN
    double peak = 0.0;
    double maxDrawdownPct = 0.0;

    for (double value : portfolioValues) {

        if (value > peak) {
            peak = value;
        }

        if (peak > 0.0) {

            double drawdown =
                ((peak - value) / peak) * 100.0;

            maxDrawdownPct =
                std::max(maxDrawdownPct, drawdown);
        }
    }

    result.maxDrawdown = maxDrawdownPct;

    return result;
}