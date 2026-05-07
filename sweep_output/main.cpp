
//all relevant libraries
#include <iostream>
#include <string>
#include <vector>
#include <limits>

//all relevant project headers
#include "PriceHistory.h"
#include "Stock.h"
#include "ETF.h"
#include "StockManager.h"
#include "Portfolio.h"
#include "StockBST.h"
//strategies
#include "FixedSIPStrategy.h"
#include "DynamicSIPStrategy.h"
#include "GoldenCrossStrategy.h"
#include "MomentumStrategy.h"

//namespaces
using std::cin;
using std::cout;
using std::string;
using std::vector;

StockManager<Stock> stockManager;   //manages stock data and price history
StockManager<ETF> etfManager;   //manages ETF data and price history

Portfolio portfolio(100000.0);  //initial cash balance for trading simulation

StockBST performanceBST; //BST to store annual return performance for stocks and ETFs for quick lookup and range queries

//input helper functions

//for clearing input buffer in terminal to ensure clean input reading
void clearInput() {
    cin.clear();
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

//read a line from user with prompt given
string readLine(const string& prompt) {
    cout << prompt;

    string input;
    std::getline(cin, input);
    return input;
}

//read an integer from user with prompt given
//validate integer
int readInt(const string& prompt) {
    int value;

    while (true) {
        cout << prompt;

        if (cin >> value) {
            clearInput();
            return value;
        }

        cout << "Invalid integer.\n";
        clearInput();
    }
}

//same idea as integer helper
double readDouble(const string& prompt) {
    double value;

    while (true) {
        cout << prompt;

        if (cin >> value) {
            clearInput();
            return value;
        }

        cout << "Invalid number.\n";
        clearInput();
    }
}

//helper function to find an asset (etf or stock) by ticker
Stock* findAsset(const string& ticker) {

    Stock* stock = stockManager.findByTicker(ticker);

    if (stock != nullptr) {
        return stock;
    }

    ETF* etf = etfManager.findByTicker(ticker);

    return etf;
}

//user will see this on startup and after each executed action
void printMenu() {

    cout << "\n========== StockSim ==========\n";

    cout << "1. Load CSV Data\n";
    cout << "2. Display Price History\n";
    cout << "3. Search Date Range\n";

    cout << "4. Insert Into BST\n";
    cout << "5. Display BST\n";
    cout << "6. BST Range Search\n";

    cout << "7. Buy Shares\n";
    cout << "8. Sell Shares\n";
    cout << "9. Queue Order\n";
    cout << "10. Execute Order\n";
    cout << "11. Undo Last Trade\n";

    cout << "12. Run Strategy Simulation\n";
    cout << "13. Compare Strategies\n";

    cout << "14. Portfolio Summary\n";
    cout << "15. Trade History\n";

    cout << "0. Exit\n";
}

//1
void menuLoadCSV() {
    //we want to load all CSV Data
    cout << "\n---------------------------- Load CSV Data ----------------------------\n";

    //get all the necessary info to load the data and create the asset
    string ticker = readLine("Ticker: ");
    string name = readLine("Name: ");
    string path = readLine("CSV Path: ");
    string type = readLine("Type (STOCK or ETF): ");

    if (type == "ETF") {
        //if etf (spx) then also get the expense ratio and create an ETF asset instead of stock asset
        double ratio = readDouble("Expense Ratio: ");
        ETF* etf = new ETF(ticker, name, ratio);
        etf->loadFromCSV(path);
        etfManager.addAsset(etf);
    } else {
        //otherwise just create a stock asset and load the data into it
        Stock* stock = new Stock(ticker, name);
        stock->loadFromCSV(path);
        stockManager.addAsset(stock);
    }

    cout << "Data loaded.\n";
}

//2
void menuDisplayHistory() {
    cout << "\n---------------------------- Display History ----------------------------\n";
    //get ticker and asset to display history for
    string ticker = readLine("Ticker: ");
    Stock* asset = findAsset(ticker);
    
    //check if asset exists
    if (asset == nullptr) {
        cout << "Ticker not found.\n";
        return;
    }

    //get the history
    PriceHistory* history = asset->getHistory();

    //check if history exists
    if (history == nullptr) {
        cout << "No history loaded.\n";
        return;
    }

    //print the history 
    history->printForward();
}

//3
void menuDateRange() {

    cout << "\n---------------------------- Date Range Search ----------------------------\n";
    //get ticker and asset
    string ticker = readLine("Ticker: ");
    Stock* asset = findAsset(ticker);

    //check if asset exists
    if (asset == nullptr) {
        cout << "Ticker not found.\n";
        return;
    }

    //get the history
    PriceHistory* history = asset->getHistory();
    string start = readLine("Start Date: ");
    string end = readLine("End Date: ");

    //print the history for the date range
    asset->getHistory()->printRange(start, end);
}

//4
void menuBSTRange() {
    cout << "\n---------------------------- BST Range Search ----------------------------\n";

    //get the range for annual return search
    double low = readDouble("Minimum Return %: ");
    double high = readDouble("Maximum Return %: ");

    //perform the range search on the BST and print the results
    vector<BSTNode*> results;
    performanceBST.rangeSearch(low, high, results);

    //print the results of the range search 
    for (int i = 0; i < static_cast<int>(results.size()); i++) 
        cout << results[i]->ticker << "  " << results[i]->year << "  " << results[i]->key << "%\n";
}

//5
void menuBSTInsert() {
    cout << "\n---------------------------- Insert Into BST ----------------------------\n";
   
    //get ticker and asset 
    string ticker = readLine("Ticker: ");
    Stock* asset = findAsset(ticker);

    //check if asset exists
    if (asset == nullptr) {
        cout << "Ticker not found.\n";
        return;
    }

    //get year for annual return calculation and BST insertion
    int year = readInt("Year: ");
    double annualReturn = asset->calculateAnnualReturn(year);
    performanceBST.insert(ticker, annualReturn, year);  //insert annual return to BST

    cout << "Inserted into BST.\n";
}

//6
void menuBSTDisplay() {
    cout << "\n---------------------------- BST Inorder Traversal ----------------------------\n";
    performanceBST.inorder();
}

//7
void menuBuyShares() {
    cout << "\n---------------------------- Buy Shares ----------------------------\n";

    //get all necessary info to buy shares and execute the buy in the portfolio
    string ticker = readLine("Ticker: ");
    int shares = readInt("Shares: ");
    double price = readDouble("Price: ");
    string date = readLine("Date: ");

    //buy shares in portfolio based on user input
    portfolio.buyShares(ticker, shares, price, date);
}

//8
void menuSellShares() {
    cout << "\n---------------------------- Sell Shares ----------------------------\n";

    //get all necessary info to sell shares and execute the sell in the portfolio
    string ticker = readLine("Ticker: ");
    int shares = readInt("Shares: ");
    double price = readDouble("Price: ");
    string date = readLine("Date: ");

    //sell shares in portfolio based on user input
    portfolio.sellShares(ticker, shares, price, date);
}

//9
void menuQueueOrder() {

    cout << "\n---------------------------- Queue Order ----------------------------\n";

    //set up the order we want to queue
    Order order;

    //get all necessary info to create the order and queue it in the portfolio's pending orders
    order.ticker = readLine("Ticker: ");
    order.side = readLine("BUY or SELL: ");
    order.type = readLine("MARKET or LIMIT: ");
    order.targetPrice = readDouble("Target Price: ");
    order.shares = readInt("Shares: ");
    order.submittedDate = readLine("Date: ");

    //queue that order based on all the user input
    //now it is pending for execution
    portfolio.queueOrder(order);
}

//10
void menuExecuteOrder() {
    cout << "\n---------------------------- Execute Order ----------------------------\n";
    double currentPrice = readDouble("Current Market Price: "); //execute next order in queue based on market price
    portfolio.executeNextOrder(currentPrice);
}

//11
void menuUndoTrade() {
    cout << "\n---------------------------- Undo Last Trade ----------------------------\n";
    portfolio.undoLastTrade();  //undo last executed using the trade stack in portfolio
}

//12
void menuRunStrategy() {
    cout << "\n---------------------------- Run Strategy ----------------------------\n";
    //get ticker and asset for backtesting
    string ticker = readLine("Ticker: ");
    Stock* asset = findAsset(ticker);

    //check if asset exists
    if (asset == nullptr) {
        cout << "Ticker not found.\n";
        return;
    }

    //get monthly capital and date range for backtesting
    double monthly = readDouble("Monthly Capital: ");
    int startYear = readInt("Start Year: ");
    int endYear = readInt("End Year: ");

    cout << "\n1. Fixed SIP\n";
    cout << "2. Dynamic SIP\n";
    cout << "3. Golden Cross\n";
    cout << "4. Momentum\n";

    int choice = readInt("Choice: ");

   TradingStrategy* strategy = nullptr; //base pointer to hold the chosen strategy instance for backtesting

    switch (choice) {
        case 1: strategy = new FixedSIPStrategy(); break;
        case 2: strategy = new DynamicSIPStrategy(10.0, 10.0, 2.0); break;
        case 3: strategy = new GoldenCrossStrategy(); break;
        case 4: strategy = new MomentumStrategy(5.0); break;
        default: cout << "Invalid choice.\n"; return;
    }

    //check if strategy choice is valid
    if (strategy == nullptr) {
        cout << "Invalid strategy.\n";
        return;
    }

    //get the simulation result from the strategy backtest and print it
    SimResult result = strategy->backtest(asset->getHistory(), monthly,startYear, endYear);
    strategy->printResult(result);

    delete strategy; //clean up strategy instance after use
}

//13
void menuCompareStrategies() {
    cout << "\n------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- Compare Strategies ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \n";
    //get ticker and asset for backtesting
    string ticker = readLine("Ticker: ");
    Stock* asset = findAsset(ticker);

    //check if asset exists
    if (asset == nullptr) {
        cout << "Ticker not found.\n";
        return;
    }
    
    //get monthly capital and date range for backtesting
    double monthly = readDouble("Monthly Capital: ");   
    int startYear = readInt("Start Year: ");     
    int endYear = readInt("End Year: ");

    //define strategy instances to compare
    FixedSIPStrategy fixed;
    DynamicSIPStrategy dynamic(10.0, 10.0, 2.0);
    GoldenCrossStrategy golden;
    MomentumStrategy momentum(5.0);

    //store strategy pointers in array for easy iteration and comparison
    TradingStrategy* strategies[4] = {&fixed, &dynamic, &golden, &momentum};

    //now backtest each strategy and print results for comparison
    for (int i = 0; i < 4; i++) {
        SimResult result = strategies[i]->backtest(asset->getHistory(), monthly, startYear, endYear);
        strategies[i]->printResult(result);
        cout << "\n";
    }
}

//14
void menuPortfolioSummary() {
    cout << "\n---------------------------- Portfolio Summary ----------------------------\n";
    portfolio.printHoldings();
}

//15
void menuTradeHistory() {
    cout << "\n---------------------------- Trade History ----------------------------\n";
    portfolio.printTrades();
}

//                                                      MAIN LOOP
//------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//main function simplified with all abstractions
int main() {
    //user choice variable for menu loop
    int choice = -1;

    while (choice != 0) {   //repeats until user chooses to exit
        printMenu();
        choice = readInt("\nEnter choice: ");
        //based on the choice, we prepare all the coressponding actions available 
        switch (choice) {
            case 1: menuLoadCSV(); break;
            case 2: menuDisplayHistory(); break;
            case 3: menuDateRange(); break;
            case 4: menuBSTInsert(); break;
            case 5: menuBSTDisplay(); break;
            case 6: menuBSTRange(); break;
            case 7: menuBuyShares(); break;
            case 8: menuSellShares(); break;
            case 9: menuQueueOrder(); break;
            case 10: menuExecuteOrder(); break;
            case 11: menuUndoTrade(); break;
            case 12: menuRunStrategy(); break;
            case 13: menuCompareStrategies(); break;
            case 14: menuPortfolioSummary(); break;
            case 15: menuTradeHistory(); break;
            case 0: cout << "\nExiting...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    }

    return 0;
}
