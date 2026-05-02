#include "ETF.h"
#include "CSVParser.h"
#include <iostream>
#include <iomanip>
#include <cmath>

using std::cout;
using std::string;

//ETF constructor which also uses Stock
ETF::ETF(const string& ticker, const string& name, double expenseRatio)
    : Stock(ticker, name), expenseRatio(expenseRatio) {}

string ETF::getType() const {
    return "ETF";
}

double ETF::getExpenseRatio() const {
    //return the expense ratio with this function
    return expenseRatio;
}

void ETF::printSummary() const {
    //use summary of the stock
    Stock::printSummary();

    //require the history to print summary
    PriceHistory* history = getHistory();
    //if history extracted is no good, then exit
    if (history == nullptr || history->getSize() == 0) return;

    //print out the expense ratio
    cout << "Expense Ratio: " << expenseRatio * 100.0 << "%\n";

    //10-Year CAGR
    //walk forward to find the node closest to 10 years before the last node
    PriceNode* lastNode  = nullptr; //last occurence (latest)
    PriceNode* startNode = nullptr; //first occurence (start)
    //find last node by using iterator and walking through the history
    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) lastNode = &(*it);
    //no encounter then exit
    if (lastNode == nullptr) return;

    //compute the year 10 years before the last date
    int lastYear  = CSVParser::extractYear(lastNode->date);
    int startYear = lastYear - 10;

    //scan from the beginning to find the first node in the target year
    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& n = *it;
        int y = CSVParser::extractYear(n.date);
        if (y == startYear) {
            if (startNode == nullptr) startNode = &n;  //found first 
        }
        if (y > startYear) break;  //stop since we went past the target year
    }

    //organize the totality of the information 
    if (startNode == nullptr || startNode->close <= 0.0 || lastNode->close <= 0.0) {
        cout << "10-yr CAGR    : N/A (insufficient data)\n";
    } else {
        // use formula for CAGR = (endValue / startValue)^(1/years) - 1
        double cagr = std::pow(lastNode->close / startNode->close, 1.0 / 10.0) - 1.0;
        cout << "10-yr CAGR    : " << cagr * 100.0 << "%\n";
        cout << "  (from $" << startNode->close << " on " << startNode->date << " to $"   << lastNode->close  << " on " << lastNode->date << ")\n";
    }
    //formatting
    cout << "--------------------------------------------\n";
}