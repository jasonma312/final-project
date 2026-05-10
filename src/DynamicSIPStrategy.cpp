#include "DynamicSIPStrategy.h"
#include "CSVParser.h" 
#include "CircularQueue.h"
#include <vector>
#include <cmath>
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

    if (!history || history->getSize() == 0) return result;

    auto inRange = [&](const PriceNode& node) {
        int y = CSVParser::extractYear(node.date);
        return (y >= startYear && y <= endYear);
    };

    // =========================================================
    // PASS 1: COUNT MONTHS (BUDGET SETUP)
    // =========================================================
    int monthCount = 0;
    int prevMonth = -1, prevYear = -1;

    for (auto it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;
        if (!inRange(node)) continue;

        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);

        bool newMonth = (year != prevYear || month != prevMonth);
        if (newMonth) {
            prevYear = year;
            prevMonth = month;
            monthCount++;
        }
    }

    if (monthCount == 0) return result;

    double totalBudget = monthCount * monthlyCapital;
    double budgetRemaining = totalBudget;

    // =========================================================
    // PASS 2: SIMULATION
    // =========================================================
    double totalShares = 0.0;
    prevMonth = -1;
    prevYear = -1;

    std::vector<double> closes;
    std::vector<double> portfolioValues;

    auto safeReturn = [](double a, double b) {
        return (b == 0.0) ? 0.0 : (a - b) / b;
    };

    for (auto it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;

        int year = CSVParser::extractYear(node.date);
        int month = CSVParser::extractMonth(node.date);

        if (year < startYear) continue;
        if (year > endYear) break;

        if (!inRange(node)) continue;

        bool newMonth = (year != prevYear || month != prevMonth);

        if (newMonth) {
            prevYear = year;
            prevMonth = month;

            closes.push_back(node.close);

            double multiplier = 1.0;

            // =====================================================
            // AGGRESSIVE REGIME MODEL
            // =====================================================
            if (closes.size() >= 13) {
                double m12 = safeReturn(closes.back(), closes[closes.size() - 13]);

                double ma = 0.0;
                for (int i = closes.size() - 12; i < closes.size(); i++)
                    ma += closes[i];
                ma /= 12.0;

                double trend = (node.close - ma) / ma;

                double var = 0.0;
                for (int i = closes.size() - 12; i < closes.size(); i++) {
                    double d = closes[i] - ma;
                    var += d * d;
                }
                var /= 12.0;

                double vol = sqrt(var) / ma;

                bool deepBear   = (m12 < -0.20 || trend < -0.08 || vol > 0.25);
                bool bear       = (m12 < -0.05 || trend < -0.02);
                bool bull       = (m12 > 0.05 && trend > 0.03 && vol < 0.22);
                bool strongBull = (m12 > 0.15 && trend > 0.08 && vol < 0.18);

                if (deepBear) {
                    multiplier = 5;   // minimal exposure, still invested
                }
                else if (bear) {
                    multiplier = 3.2;
                }
                else if (bull) {
                    multiplier = 0.3;   // strong trend participation
                }
                else if (strongBull) {
                    multiplier = 0.1;   // aggressive compounding mode
                }
                else {
                    multiplier = 1;   // slight bullish bias
                }
            }

            double intendedInvest = monthlyCapital * multiplier;

            // =====================================================
            // BUDGET CONSTRAINT (NO OVERSPENDING)
            // =====================================================
            double investAmount = std::min(intendedInvest, budgetRemaining);

            if (investAmount > 0.0) {
                double shares = investAmount / node.close;
                totalShares += shares;

                result.totalInvested += investAmount;
                result.totalTrades++;

                budgetRemaining -= investAmount;
            }
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
