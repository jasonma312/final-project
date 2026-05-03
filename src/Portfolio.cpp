#include "Portfolio.h"
#include "OrderQueue.h"
#include "TradeStack.h"

#include <iostream>
#include <iomanip>
#include <algorithm>

using std::cout;
using std::string;
using std::vector;

//constructor initialises cash balance and empty holdings/orders/trades
Portfolio::Portfolio(double initialCash)
    : cashBalance(initialCash) {}

//destructor
Portfolio::~Portfolio() {}

//scan thru position holdings using ticker value, return index if found or -1 if not found
int Portfolio::findPosition(const string& ticker) const {
    for (int i = 0; i < static_cast<int>(holdings.size()); i++) {
        if (holdings[i].ticker == ticker) return i;
    }
    //otherwise not found
    return -1;
}

void Portfolio::updateAvgCost(Position& pos, int newShares, double newPrice) {
    //calculate total cost of existing shares and new shares
    //use old and new totals to compute the new average cost basis for the position
    double oldTotal = pos.avgCostBasis * pos.shares; //current total cost of existing shares
    double newTotal = newPrice * newShares; //total cost of new shares being added 

    //perform the update for cost basis
    int combined = pos.shares + newShares;

    if (combined > 0) {
        pos.avgCostBasis = (oldTotal + newTotal) / combined;
    } else {
        pos.avgCostBasis = 0.0;
    }

    //update total shares in the position
    pos.shares = combined;
}

void Portfolio::buyShares(const string& ticker, int shares, double price, const string& date) {
    //calculate total cost of the purchase
    double cost = static_cast<double>(shares) * price;

    //if not enough cash balance to afford, print error and return without executing the trade
    if (cost > cashBalance) {
        cout <<  "Insufficient cash to buy " << shares << " shares of " << ticker << " @ $" << std::fixed << std::setprecision(2) << price
             << "  (need $" << cost << ", have $" << cashBalance << ")\n";
        return;
    }

    //find the position in holdings vector for this ticker, or create a new one if it doesn't exist
    int i = findPosition(ticker);
    if (i == -1) {    //-1 is returned for non existant ticker
        //this becomes a new position if the ticker is not already held in the portfolio
        Position pos;
        pos.ticker = ticker;
        pos.shares = 0;
        pos.avgCostBasis = 0.0;
        pos.currentPrice = price;
        holdings.push_back(pos);
        i = static_cast<int>(holdings.size()) - 1;
    }

    //update with the new shares and price
    updateAvgCost(holdings[i], shares, price);
    //deduct the cost of the purchase from cash balance and update current price for the position
    holdings[i].currentPrice = price;
    cashBalance -= cost;

    //this gets recorded into the trade record history stack
    TradeRecord rec;
    rec.ticker = ticker;
    rec.date = date;
    rec.price = price;
    rec.shares = shares;
    rec.action = "BUY";
    rec.totalCost = cost;
    tradeHistory.push(rec);

    cout << "BUY  " << shares << " " << ticker << " , $" << price << "  ,  Cash remaining: $" << cashBalance << "\n";
}

void Portfolio::sellShares(const string& ticker, int shares, double price, const string& date) {
    //find what you want to sell in holdings vector
    //if not found print error and return without executing the trade
    int i = findPosition(ticker);
    if (i == -1) {
        cout << "Cannot sell " << ticker << ", position not found.\n";
        return;
    }
    //if found but not enough shares to sell, print error and return without executing the trade
    if (holdings[i].shares < shares) {
        cout << "Cannot sell " << shares << " shares of " << ticker << ", only " << holdings[i].shares << " held.\n";
        return;
    }

    //otherwise if found and sell possible, perform the following
    double proceeds = static_cast<double>(shares) * price;  //calculate total proceeds from the sale
    holdings[i].shares -= shares;    //update shares in the position by subtracting the shares sold
    holdings[i].currentPrice = price;     //update current price for the position to the sale price
    cashBalance += proceeds;  //add the proceeds from the sale to cash balance

    //position removed if all shares are sold and shares count goes to zero
    if (holdings[i].shares == 0) holdings.erase(holdings.begin() + i);

    //record down in trade record history stack
    TradeRecord rec;
    rec.ticker = ticker;
    rec.date = date;
    rec.price = price;
    rec.shares = shares;
    rec.action = "SELL";
    rec.totalCost = proceeds;
    tradeHistory.push(rec);

    cout << "SELL " << shares << " " << ticker << ", $" << price << ", Cash: $" << cashBalance << "\n";
}

void Portfolio::undoLastTrade() {
    //if there are no trades in the trade history stack, print error and return
    if (tradeHistory.isEmpty()) {
        cout << "No trades to undo.\n";
        return;
    }

    //otherwise pop the most recent trade record from the stack and reverse the trade
    TradeRecord rec = tradeHistory.pop();
    cout << " Undoing: " << rec.action << " " << rec.shares << " " << rec.ticker << ", $" << rec.price  
        << " on " << rec.date << "\n";

    if (rec.action == "BUY") {
        //to reverse a buy we need to sell the shares back, deduct shares from position, add cash back to balance
        int i = findPosition(rec.ticker);
        if (i != -1) {  //if found in holdings, then deduct the shares bought from the position
            holdings[i].shares -= rec.shares;   //reverse the shares bought by subtracting them from the position
            if (holdings[i].shares == 0) holdings.erase(holdings.begin() + i);  //if all shares are undone and shares count goes to zero, 
                                                                                //remove the position from holdings
        }
        //restore the cash spent on the purchase back to cash balance
        cashBalance += rec.totalCost;
    } else {
        //to reverse a sell we need to buy the shares back, add shares back to position, deduct cash from balance
        int i = findPosition(rec.ticker);
        if (i == -1) {  //not found in holders means it was sold
            //recreate that old position back again 
            Position pos;
            //reuse all the old trade record details to restore the old position
            pos.ticker = rec.ticker;
            pos.shares = 0;
            pos.avgCostBasis = rec.price;
            pos.currentPrice = rec.price;
            holdings.push_back(pos);
            i = static_cast<int>(holdings.size()) - 1;  //get the index of the newly created position in holdings vector
        }
        holdings[i].shares += rec.shares;   //reverse the shares sold by adding them back to the position
        cashBalance -= rec.totalCost;   //deduct the cash
    }
}

void Portfolio::queueOrder(const Order& order) {
    //add the order to the pending orders queue and print out a message confirming the order details
    pendingOrders.enqueue(order);
    cout << " Queued " << order.side << " " << order.shares << " " << order.ticker;

    //determine order type (market or limit) and print out the appropriate message
    if (order.type == "LIMIT") {    //limit order 
        cout << " LIMIT $" << order.targetPrice;
    } else {
        cout << " MARKET"; //market order 
        //target price is set to 0 for market orders. handeled by constructor
    }

    //formatting 
    cout << "\n";
}

void Portfolio::executeNextOrder(double currentPrice) {
    //case of no pending orders in the queue, then just leave
    if (pendingOrders.isEmpty()) {
        cout << " No pending orders.\n";
        return;
    }

    //get next order at front of order queue
    Order o = pendingOrders.dequeue();
    bool execute = false;

    //if market type order, then process the order and execute it immediately at the current market price
    if (o.type == "MARKET") {
        execute = true;
    } else if (o.type == "LIMIT") { //if limit type order then check if the current market price meets the target price condition to complete the order
        if (o.side == "BUY"  && currentPrice <= o.targetPrice) execute = true;  //for buy limit order, current price must be at or below the target price to execute
        if (o.side == "SELL" && currentPrice >= o.targetPrice) execute = true;  //for sell limit order, current price must be at or above the target price to execute
    }

    //non executable limit order means the order is discarded and so indiciate that, since the action failed
    if (!execute) {
        cout << " LIMIT order for " << o.ticker << " not filled, market $"<< currentPrice << " vs target $" 
            << o.targetPrice << ". Order discarded.\n";
        return;
    }

    //if executable then perform the order action of BUY or SELL at market price and print out the details of the executed trade
    if (o.side == "BUY") {
        buyShares(o.ticker, o.shares, currentPrice, o.submittedDate);
    } else {
        sellShares(o.ticker, o.shares, currentPrice, o.submittedDate);
    }
}

//refresh current price for ticker in the holdings vector
//if no such ticker then don't do anything
void Portfolio::refreshPrice(const string& ticker, double price) {
    int i = findPosition(ticker);
    //if found then update the current price for that position
    if (i != -1) holdings[i].currentPrice = price;
}

//total val of portfolio
double Portfolio::getTotalValue() const {
    double total = cashBalance; //store cash balance as part of total portfolio value
    //market value of each position in holdings and then add all to the total as we iterate thru holdings vector
    for (int i = 0; i < static_cast<int>(holdings.size()); i++) total += static_cast<double>(holdings[i].shares) * holdings[i].currentPrice;
    return total;   //return the total value
}

//total return of portfolio in percentage
double Portfolio::getTotalReturn() const {
    double costBasis = 0.0, marketValue = 0.0;  //doubles to store total cost basis and market val of portfolio
    //iterate thru holdings vector to accumulate all positions
    for (int i = 0; i < static_cast<int>(holdings.size()); i++) {
        double sh = static_cast<double>(holdings[i].shares);    //shares as double for calculation
        costBasis   += sh * holdings[i].avgCostBasis;   //accumulate cost basis
        marketValue += sh * holdings[i].currentPrice;   //accumulate market value
    }
    if (costBasis <= 0.0) return 0.0;   //no position, zero return, easy case
    return (marketValue - costBasis) / costBasis * 100.0;   //otherwise if a valid value then print out total return %
}

//cash balance
double Portfolio::getCashBalance() const { 
    return cashBalance; 
}

//print all portfolio holdings
void Portfolio::printHoldings() const {
    cout << "\n**************** Portfolio Holdings ****************\n";
    cout << "Cash Balance: $" << cashBalance << "\n\n";

    if (holdings.empty()) {
        //if we have no positions then default to this message
        cout << "  no positions held\n";
    } else {
        //table style formatting
        cout << std::left
             << std::setw(8) << "Ticker"
             << std::setw(8) << "Shares"
             << std::setw(12) << "Avg Cost"
             << std::setw(12) << "Cur Price"
             << std::setw(14) << "Mkt Value"
             << "Unrealized P/L" << "\n";
        cout << std::string(66, '-') << "\n";

        //accumulators like above
        double totalMkt  = 0.0, totalCost = 0.0;

        //go through all holdings and extract their details so we can print them in the table format
        for (int i = 0; i < static_cast<int>(holdings.size()); i++) {
            const Position& p = holdings[i];
            double sh = static_cast<double>(p.shares);
            double mktVal = sh * p.currentPrice;
            double basis = sh * p.avgCostBasis;
            double unrealized = mktVal - basis;
            totalMkt += mktVal;
            totalCost += basis;

            //populate the table with details for this specific position in holdings vector
            cout << std::left
                 << std::setw(8)  << p.ticker
                 << std::setw(8)  << p.shares
                 << "$" << std::setw(11) << p.avgCostBasis
                 << "$" << std::setw(11) << p.currentPrice
                 << "$" << std::setw(13) << mktVal
                 << (unrealized >= 0.0 ? "+" : "") << "$" << unrealized //simplify using ternerary operator
                 << "\n";
        }

        //print out the totals for the portfolio at the bottom of the table
        cout << std::string(66, '-') << "\n";
        double totalUnrealized = totalMkt - totalCost;
        cout << std::left
             << std::setw(28) << "TOTAL POSITIONS"
             << "$" << std::setw(13) << totalMkt
             << (totalUnrealized >= 0.0 ? "+" : "") << "$" << totalUnrealized //simplify using ternerary operator
             << "\n";
    }

    //print out the total portfolio value, total return at the end
    cout << "\nTotal Portfolio Value: $" << getTotalValue() << "\n";
    cout << "Total Return:  " << getTotalReturn() << "%\n";
    cout << "****************************************************************\n";
}

//print orders in the pending orders queue
void Portfolio::printOrders() const {
    cout << "\n******** Pending Orders ********\n";
    pendingOrders.printAll();
}

//print trades in the trade history stack to see trades
void Portfolio::printTrades() const {
    cout << "\n******** Trade History ********\n";
    tradeHistory.printAll();
}

//unrealized gain/loss in descending order
void Portfolio::sortHoldingsByReturn() {   
    //use std sort to help with sorting
    //custom comparator given below
    std::sort(holdings.begin(), holdings.end(), 
        [](const Position& a, const Position& b) 
        {
            double gainA = (a.currentPrice - a.avgCostBasis) * a.shares;
            double gainB = (b.currentPrice - b.avgCostBasis) * b.shares;
            return gainA > gainB;
        });
}

//ticker symbol order in ascending alphabetical order
void Portfolio::sortHoldingsByTicker() {
    //use std sort to help with sorting
    //custom comparator given below
    std::sort(holdings.begin(), holdings.end(), 
        [](const Position& a, const Position& b) 
        {
            return a.ticker < b.ticker;
        });
}