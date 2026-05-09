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
    return "Momentum Strategy";
}

// backtest using the momentum strategy with the given momentum threshold
// SimResult MomentumStrategy::backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) {
//     SimResult result;
//     result.strategyName = getName();
//     result.finalValue = 0.0;
//     result.totalInvested = 0.0;
//     result.totalReturn = 0.0;
//     result.cagr = 0.0;
//     result.maxDrawdown = 0.0;
//     result.totalTrades = 0;

//     if (history == nullptr || history->getSize() == 0)
//         return result;

//     double totalShares = 0.0;

//     int prevMonth = -1;
//     int prevYear = -1;

//     double prevMonthClose = 0.0;   // FIX: proper momentum baseline

//     vector<double> portfolioValues;

//     for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
//         PriceNode& node = *it;

//         int year = CSVParser::extractYear(node.date);
//         int month = CSVParser::extractMonth(node.date);

//         if (year < startYear) continue;
//         if (year > endYear) break;

//         bool newMonth = (year != prevYear || month != prevMonth);

//         if (newMonth) {
//             prevYear = year;
//             prevMonth = month;

//             // FIX: skip first valid month (no prior data)
//             if (prevMonthClose == 0.0) {
//                 prevMonthClose = node.close;
//             } else {

//                 // FIX: true monthly momentum (NOT daily noise)
//                 double momentum =
//                     (node.close - prevMonthClose) / prevMonthClose * 100.0;

//                 if (momentum > momentumThreshold) {
//                     double sharesToBuy = monthlyCapital / node.close;
//                     totalShares += sharesToBuy;

//                     result.totalInvested += monthlyCapital;
//                     result.totalTrades++;
//                 }

//                 // update baseline AFTER decision
//                 prevMonthClose = node.close;
//             }
//         }

//         double portfolioValue = totalShares * node.close;
//         portfolioValues.push_back(portfolioValue);

//         result.finalValue = portfolioValue;
//     }

//     if (result.totalInvested > 0) {
//         result.totalReturn =
//             (result.finalValue - result.totalInvested) / result.totalInvested * 100.0;
//     } else {
//         result.totalReturn = 0.0;
//     }

//     int years = std::max(1, endYear - startYear + 1);
//     result.cagr = calculateCAGR(result.totalInvested, result.finalValue, years);
//     result.maxDrawdown = calculateMaxDrawdown(portfolioValues);

//     return result;
// }

SimResult MomentumStrategy::backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) {
    SimResult result;

    result.strategyName = getName();
    result.finalValue = 0.0;
    result.totalInvested = 0.0;
    result.totalReturn = 0.0;
    result.cagr = 0.0;
    result.maxDrawdown = 0.0;
    result.totalTrades = 0;

    if (!history || history->getSize() == 0)
        return result;

    double totalShares = 0.0;

    int prevMonth = -1;
    int prevYear = -1;

    double prevMonthClose = 0.0;

    // manual rolling window (size = 4 points = 3-month momentum)
    vector<double> closes;

    vector<double> portfolioValues;

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

            // initialize
            if (prevMonthClose == 0.0) {
                prevMonthClose = close;
                closes.push_back(close);
                continue;
            }

            // maintain fixed-size window manually
            closes.push_back(close);
            if (closes.size() > 4) {
                // shift manually (no deque allowed)
                for (size_t i = 1; i < closes.size(); i++) {
                    closes[i - 1] = closes[i];
                }
                closes.pop_back();
            }

            double allocation = 1.0;

            // compute momentum only if we have enough history
            if (closes.size() == 4) {
                double past = closes[0];
                double current = closes[3];

                double momentum =
                    (current - past) / past * 100.0;

                if (momentum > momentumThreshold) {
                    allocation = 1.0 + (momentum / 100.0);
                } else {
                    allocation = 0.5; // still invest (SIP baseline)
                }

                // clamp to avoid extreme leverage
                if (allocation > 2.0) allocation = 2.0;
                if (allocation < 0.2) allocation = 0.2;
            }

            double investment = monthlyCapital * allocation;
            double shares = investment / close;

            totalShares += shares;

            result.totalInvested += investment;
            result.totalTrades++;

            prevMonthClose = close;
        }

        double portfolioValue = totalShares * node.close;
        portfolioValues.push_back(portfolioValue);

        result.finalValue = portfolioValue;
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