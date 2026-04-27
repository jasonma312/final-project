#include "Stock.h"
#include "CSVParser.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

using std::cout;
using std::string;

//constructor and destructors
Stock::Stock(const string& ticker, const string& name) : FinancialAsset(ticker, name), history(nullptr) {}

Stock::~Stock() {
    delete history;
    history = nullptr;
}


//load history from CSV file
void Stock::loadFromCSV(const string& filename) {
    //delete any existing
    delete history;
    //load the history 
    history = CSVParser::loadHistory(filename);
    //check if any errors
    if (history == nullptr) cout << "Failed to load data for " << ticker << "\n";
}

//get the history of the stock
PriceHistory* Stock::getHistory() const {
    return history;
}

//extract price for specific date
double Stock::getPriceOnDate(const std::string& date) const {
    //no history - error
    if (history == nullptr) return -1.0;

    //find the date
    PriceNode* node = history->findByDate(date);
    //if nothing then error/exit
    if (node == nullptr) return -1.0;

    //return the price
    return node->close;
}

//calcute the annual return 
double Stock::calculateAnnualReturn(int year) const {
    //base case - no history no calculation
    if (history == nullptr) return 0.0;

    //find the first node in that year
    PriceNode* firstNode = nullptr;
    //find the last node in that year
    PriceNode* lastNode  = nullptr;

    //walk thru the history of that year and extract the first and last of that year 
    //use the iterator to help with this walk
    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& node = *it;
        if (CSVParser::extractYear(node.date) == year) {    //if of that year
            if (firstNode == nullptr) firstNode = &node;    //first node of that year is assigned at once
            lastNode = &node;                               //keep reassigning, when we reach the end this wont update anymore
        }
    }

    //handle null case
    if (firstNode == nullptr || lastNode == nullptr) return 0.0;
    if (firstNode->close == 0.0) return 0.0;

    //return result of the formulation
    return (lastNode->close - firstNode->close) / firstNode->close * 100.0;
}

void Stock::printSummary() const {
    //proint information about stock
    cout << "--------------------------------------------\n";
    cout << "Type   : " << getType() << "\n";
    cout << "Ticker : " << ticker << "\n";
    cout << "Name   : " << name << "\n";
    cout << "Sector : " << sector << "\n";

    //check if data exists before moving on 
    if (history == nullptr || history->getSize() == 0) {
        cout << "  No price data loaded\n";
        cout << "--------------------------------------------\n";
        return;
    }

    //get the first and last using the same structure as in calculateAnnualReturn but repurposed for this application
    PriceNode* first = nullptr;
    PriceNode* last  = nullptr;
    for (PriceHistory::Iterator it = history->begin(); it != history->end(); ++it) {
        PriceNode& n = *it;
        if (first == nullptr) first = &n;
        last = &n;
    }

    //print date range and size from that walk
    string fromDate = "N/A";
    string toDate   = "N/A";
    //if no node then no assignment
    if (first != nullptr) fromDate = first->date;
    if (last  != nullptr) toDate   = last->date;

    //printing that information
    cout << "From   : " << fromDate << "\n";
    cout << "To     : " << toDate << "\n";
    cout << "Days   : " << history->getSize() << " trading days\n";

    //compute only if we have the valid information 
    if (first != nullptr && last != nullptr && first->close > 0.0) {
        //provide the return using the formula 
        double totalReturn = (last->close - first->close) / first->close * 100.0;

        //fix and print out
        cout << std::fixed << std::setprecision(2);
        cout << "Return : " << totalReturn << "% (full period)\n";
        cout << "First  : $" << first->close << "\n";
        cout << "Last   : $" << last->close << "\n";
    }
    cout << "--------------------------------------------\n";
}

string Stock::getType() const {
    return "Stock";
}