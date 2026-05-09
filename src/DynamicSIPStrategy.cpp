#include "DynamicSIPStrategy.h"
#include "CSVParser.h" 
#include <vector>
#include <cmath>
using std::string;


DynamicSIPStrategy::DynamicSIPStrategy(double base, double inc, double dec) : baseAmount(base), increasePct(inc), decreasePct(dec) {}
DynamicSIPStrategy::~DynamicSIPStrategy() {}

string DynamicSIPStrategy::getName() const {
    return "Dynamic SIP";
}

// SimResult DynamicSIPStrategy::backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) {
//     SimResult result;
//     result.strategyName = getName();
//     result.finalValue = 0.0;
//     result.totalInvested = 0.0;
//     result.totalReturn = 0.0;
//     result.cagr = 0.0;
//     result.maxDrawdown = 0.0;
//     result.totalTrades = 0;

//     if (history == nullptr || history->getSize() == 0) return result;

//     double totalShares = 0.0;
//     int prevMonth = -1; 
//     int prevYear = -1;
//     double lastClose = 0.0;

//     std::vector<double> portfolioValues;

//     for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
//         PriceNode& node = *it;
//         int year = CSVParser::extractYear(node.date);
//         int month = CSVParser::extractMonth(node.date);

//         if (year < startYear) continue;
//         if (year > endYear) break;

//         bool newMonth = (year != prevYear || month != prevMonth);

//         if (newMonth) {
//             prevYear  = year;
//             prevMonth = month;

//             double investAmount = baseAmount;

//             if (lastClose > 0) {
//                 double priceChangePct = (node.close - lastClose) / lastClose * 100.0;

//                 if (priceChangePct < 0) {
//                     investAmount += investAmount * (increasePct / 100.0);
//                 } else if (priceChangePct > 0) {
//                     investAmount -= investAmount * (decreasePct / 100.0);
//                 }
//             }

//             investAmount = std::max(0.0, investAmount); // Ensure we don't invest a negative amount

//             double sharesToBuy = investAmount / node.close;
//             totalShares += sharesToBuy;
//             result.totalInvested += investAmount;
//             result.totalTrades++;

//             lastClose = node.close; //update the last close for the next iteration
//         }

//         double portfolioValue = totalShares * node.close;
//         portfolioValues.push_back(portfolioValue);
//     }

//     if (!portfolioValues.empty()) {
//         result.finalValue = portfolioValues.back();
//         result.totalReturn = result.totalInvested > 0 ? (result.finalValue - result.totalInvested) / result.totalInvested * 100.0 : 0.0;
//         int years = endYear - startYear + 1;
//         if (years > 0) {
//             result.cagr = calculateCAGR(result.totalInvested, result.finalValue, years);
//             result.maxDrawdown = calculateMaxDrawdown(portfolioValues);
//         }
//     }

//     return result;
// }

SimResult DynamicSIPStrategy::backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) {
    SimResult result;
    result.strategyName = getName();
    result.finalValue = 0.0;
    result.totalInvested = 0.0;
    result.totalReturn = 0.0;
    result.cagr = 0.0;
    result.maxDrawdown = 0.0;
    result.totalTrades = 0;

    if (!history || history->getSize() == 0) return result;

    double totalShares = 0.0;
    int prevMonth = -1, prevYear = -1;

    std::vector<double> closes;
    std::vector<double> portfolioValues;

    double peakValue = 0.0;

    auto safeReturn = [](double a, double b) {
        return (b == 0.0) ? 0.0 : (a - b) / b;
    };

    for (auto it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;

        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);

        if (year < startYear) continue;
        if (year > endYear) break;

        bool newMonth = (year != prevYear || month != prevMonth);

        if (newMonth) {
            prevYear = year;
            prevMonth = month;

            closes.push_back(node.close);

            double multiplier = 1.0;

            // =========================
            // 1. SHORT MOMENTUM (3M)
            // =========================
            if (closes.size() >= 4) {
                double ms = safeReturn(closes.back(), closes[closes.size() - 4]);

                if (ms > 0.08) multiplier += 0.7;
                else if (ms < 0.0) multiplier -= 0.6;
            }

            // =========================
            // 2. LONG MOMENTUM (12M)
            // =========================
            if (closes.size() >= 13) {
                double ml = safeReturn(closes.back(), closes[closes.size() - 13]);

                if (ml > 0.15) multiplier += 0.8;
                else if (ml < 0.0) multiplier -= 0.8;
            }

            // =========================
            // 3. TREND FILTER (MA12)
            // =========================
            if (closes.size() >= 12) {
                double ma = 0.0;
                for (int i = closes.size() - 12; i < closes.size(); i++)
                    ma += closes[i];
                ma /= 12;

                double trend = (node.close - ma) / ma;

                if (trend > 0.0) multiplier += 0.5;
                else multiplier -= 0.5;
            }

            // =========================
            // 4. VOLATILITY FILTER
            // =========================
            if (closes.size() >= 12) {
                double mean = 0.0;
                for (int i = closes.size() - 12; i < closes.size(); i++)
                    mean += closes[i];
                mean /= 12;

                double var = 0.0;
                for (int i = closes.size() - 12; i < closes.size(); i++) {
                    double d = closes[i] - mean;
                    var += d * d;
                }
                var /= 12;

                double vol = std::sqrt(var) / mean;

                if (vol > 0.18) multiplier -= 0.7;
            }

            // =========================
            // 5. CRASH BOOST (HIGH RISK FEATURE)
            // =========================
            double currentValue = totalShares * node.close;
            if (currentValue > peakValue) peakValue = currentValue;

            double drawdown = (peakValue == 0.0) ? 0.0 : (currentValue - peakValue) / peakValue;

            if (drawdown < -0.30) {
                multiplier += 0.8; // aggressive crash buying
            }

            // =========================
            // FINAL CLAMP (ALLOW OVEREXPOSURE)
            // =========================
            if (multiplier < 0.0) multiplier = 0.0;
            if (multiplier > 2.5) multiplier = 2.5;

            double investAmount = monthlyCapital * multiplier;

            double shares = investAmount / node.close;
            totalShares += shares;

            result.totalInvested += investAmount;
            result.totalTrades++;
        }

        double portfolioValue = totalShares * node.close;
        portfolioValues.push_back(portfolioValue);
    }

    if (portfolioValues.empty()) return result;

    result.finalValue = portfolioValues.back();

    if (result.totalInvested > 0) {
        result.totalReturn =
            (result.finalValue - result.totalInvested) /
            result.totalInvested * 100.0;
    }

    int years = endYear - startYear + 1;
    result.cagr = calculateCAGR(result.totalInvested, result.finalValue, years);

    result.maxDrawdown = calculateMaxDrawdown(portfolioValues);

    return result;
}