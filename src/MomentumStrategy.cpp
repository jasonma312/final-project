#include "MomentumStrategy.h"
#include "CSVParser.h"
#include <string> 

using std::string; 

MomentumStrategy::MomentumStrategy(double mt) : momentumThreshold(mt) {}
MomentumStrategy::~MomentumStrategy() {}

SimResult MomentumStrategy::backtest(PriceHistory* history, double monthlyCapital, int startYear, int endYear) {
    SimResult result;
    result.strategyName = getName();

    if (!history || history->getSize() < 130) return result;

    double cash = 0.0;
    double shares = 0.0;
    double totalInvested = 0.0;
    int totalTrades = 0;

    std::vector<double> portfolioValues;

    int dayCounter = 0;

    // iterate forward through prices
    for (auto it = history->begin(); it != history->end(); ++it) {
        PriceNode& current = *it;
        dayCounter++;

        // simulate monthly step (~21 trading days)
        if (dayCounter % 21 != 0) continue;

        // add capital monthly
        cash += monthlyCapital;
        totalInvested += monthlyCapital;

        // --- compute 6-month momentum using reverse iterator ---
        auto rit = history->rbegin();

        // find current node in reverse traversal
        while (rit != history->rend() && &(*rit) != &current) {
            ++rit;
        }

        // go back ~126 trading days
        int steps = 0;
        while (rit != history->rend() && steps < 126) {
            ++rit;
            steps++;
        }

        if (!(rit != history->rend())) continue;

        double pastPrice = (*rit).close;
        double currentPrice = current.close;

        if (pastPrice <= 0) continue;

        double momentum = (currentPrice - pastPrice) / pastPrice;

        // --- trading logic ---
        if (momentum > momentumThreshold) {
            // BUY (use all cash)
            if (cash > 0) {
                shares += cash / currentPrice;
                cash = 0;
                totalTrades++;
            }
        } else if (momentum < 0) {
            // SELL (liquidate)
            if (shares > 0) {
                cash += shares * currentPrice;
                shares = 0;
                totalTrades++;
            }
        }

        // track portfolio value
        double value = cash + shares * currentPrice;
        portfolioValues.push_back(value);
    }

    double finalPrice = 0.0;

    // get last price
    for (auto it = history->rbegin(); it != history->rend(); ++it) {
        finalPrice = (*it).close;
        break;
    }

    double finalValue = cash + shares * finalPrice;

    result.finalValue = finalValue;
    result.totalInvested = totalInvested;
    result.totalReturn = (totalInvested > 0)
        ? (finalValue - totalInvested) / totalInvested * 100.0
        : 0.0;

    int years = endYear - startYear + 1;
    result.cagr = calculateCAGR(totalInvested, finalValue, years);
    result.maxDrawdown = calculateMaxDrawdown(portfolioValues);
    result.totalTrades = totalTrades;

    return result;
}

string MomentumStrategy::getName() const {
    return "6-Month Momentum"; 
}
