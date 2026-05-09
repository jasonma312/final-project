#include "MomentumStrategy.h"
#include "CSVParser.h"

#include <vector>
#include <cmath>

using std::string;
using std::vector;
using std::pow;

MomentumStrategy::MomentumStrategy(double mt) : momentumThreshold(mt) {}
MomentumStrategy::~MomentumStrategy() {}

string MomentumStrategy::getName() const {
    return "6-Month Momentum";
}

// Buys when recent momentum is positive, sells/stays cash when negative: - Every month,
// compute 6-month trailing return using ReverseIterator on PriceHistory - Buy if trailing
// return > momentumThreshold; sell if below 0 - Rebalance monthly 
SimResult MomentumStrategy::backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) {
    // Creates Sim Result structure to store results of the backtest 
    SimResult result;
    result.strategyName = getName();
    result.finalValue = 0.0;
    result.totalInvested = 0.0;
    result.totalReturn = 0.0;
    result.cagr = 0.0;
    result.maxDrawdown = 0.0;
    result.totalTrades = 0;

    // Edge case: no history data, return empty result 
    if (!history || history->getSize() == 0) return result;

    double totalShares = 0.0;
    bool invested; 
    vector<double> closes;
    vector<double> portfolioValues;
    int prevMonth = -1;
    int prevYear = -1;

    // store monthly closes in forward order

    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;

        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);

        if (year < startYear) continue;
        if (year > endYear) break;

        bool newMonth = (year != prevYear || month != prevMonth);

        if (newMonth) {
            prevYear = year;
            prevMonth = month;

            double close = node.close;
            closes.push_back(close);

            double allocation = 0.0; // default = cash

            // need at least 7 months (6-month return window)
            if (closes.size() >= 7) {

                double pastPrice = closes[closes.size() - 7]; // 6 months ago
                double currentPrice = closes.back();

                double momentum =
                    (currentPrice - pastPrice) / pastPrice * 100.0;

                // BUY condition
                if (momentum > momentumThreshold) {
                    allocation = 1.0;  // fully invested
                }
                // SELL / CASH condition
                else if (momentum < 0.0) {
                    allocation = 0.0;  // move to cash
                }
                // neutral zone
                else {
                    allocation = 0.5;  // partial exposure
                }
            } else {
                // not enough history → stay invested lightly
                allocation = 1.0;
            }

            double targetValue = monthlyCapital * allocation;

            if (allocation > 0.0) {
                double sharesToBuy = targetValue / close;
                totalShares += sharesToBuy;

                result.totalInvested += targetValue;
                result.totalTrades++;
            }

            // portfolio value update
            double portfolioValue = totalShares * close;
            portfolioValues.push_back(portfolioValue);

            result.finalValue = portfolioValue;
        }
    }

    if (result.totalInvested > 0) {
        result.totalReturn =
            (result.finalValue - result.totalInvested) / result.totalInvested * 100.0;
    }

    int years = std::max(1, endYear - startYear + 1);
    result.cagr = calculateCAGR(result.totalInvested, result.finalValue, years);
    result.maxDrawdown = calculateMaxDrawdown(portfolioValues);

    return result;
}