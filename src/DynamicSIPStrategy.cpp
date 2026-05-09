#include "DynamicSIPStrategy.h"
#include "CSVParser.h" 
#include <vector>

using std::string;


DynamicSIPStrategy::DynamicSIPStrategy(double base, double inc, double dec) : baseAmount(base), increasePct(inc), decreasePct(dec) {}
DynamicSIPStrategy::~DynamicSIPStrategy() {}

string DynamicSIPStrategy::getName() const {
    return "Dynamic SIP";
}

SimResult DynamicSIPStrategy::backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) {
    SimResult result;
    result.strategyName = getName();
    result.finalValue = 0.0;
    result.totalInvested = 0.0;
    result.totalReturn = 0.0;
    result.cagr = 0.0;
    result.maxDrawdown = 0.0;
    result.totalTrades = 0;

    if (history == nullptr || history->getSize() == 0) return result;

    double totalShares = 0.0;
    int prevMonth = -1; 
    int prevYear = -1;
    double lastClose = 0.0;

    std::vector<double> portfolioValues;

    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;
        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);

        if (year < startYear) continue;
        if (year > endYear) break;

        bool newMonth = (year != prevYear || month != prevMonth);

        if (newMonth) {
            prevYear  = year;
            prevMonth = month;

            double investAmount = baseAmount;

            if (lastClose > 0) {
                double priceChangePct = (node.close - lastClose) / lastClose * 100.0;

                if (priceChangePct < 0) {
                    investAmount += investAmount * (increasePct / 100.0);
                } else if (priceChangePct > 0) {
                    investAmount -= investAmount * (decreasePct / 100.0);
                }
            }

            investAmount = std::max(0.0, investAmount); // Ensure we don't invest a negative amount

            double sharesToBuy = investAmount / node.close;
            totalShares += sharesToBuy;
            result.totalInvested += investAmount;
            result.totalTrades++;

            lastClose = node.close; //update the last close for the next iteration
        }

        double portfolioValue = totalShares * node.close;
        portfolioValues.push_back(portfolioValue);
    }

    if (!portfolioValues.empty()) {
        result.finalValue = portfolioValues.back();
        result.totalReturn = result.totalInvested > 0 ? (result.finalValue - result.totalInvested) / result.totalInvested * 100.0 : 0.0;
        int years = endYear - startYear + 1;
        if (years > 0) {
            result.cagr = calculateCAGR(result.totalInvested, result.finalValue, years);
            result.maxDrawdown = calculateMaxDrawdown(portfolioValues);
        }
    }

    return result;
}