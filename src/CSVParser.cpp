#include "CSVParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

using std::string;
using std::ifstream;
using std::istringstream;
using std::getline;
using std::cerr;
using std::cout;

//load the file data into price history linked list
PriceHistory* CSVParser::loadHistory(const string& filename) {
    //open file and  check
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open file: " << filename << "\n";
        return nullptr;
    }

    //load the history into here
    PriceHistory* history = new PriceHistory();
    //line
    string line;

    //empty case
    if (!getline(file, line)) {
        cerr << "File is empty: " << filename << "\n";
        delete history;
        return nullptr;
    }

    int rowsParsed = 0;
    int rowsSkipped = 0;

    while (getline(file, line)) {
        if (line.empty()) continue;

        istringstream ss(line);

        string dateStr, openStr, highStr, lowStr, closeStr, adjCloseStr, volumeStr;

        //get the data
        if (!getline(ss, dateStr, ',') ||
            !getline(ss, openStr, ',') ||
            !getline(ss, highStr, ',') ||
            !getline(ss, lowStr, ',') ||
            !getline(ss, closeStr, ',') ||
            !getline(ss, adjCloseStr, ',') ||
            !getline(ss, volumeStr, ',')) {
            rowsSkipped++;
            continue;
        }

        //conversion and add
        double open = atof(openStr.c_str());
        double high = atof(highStr.c_str());
        double low = atof(lowStr.c_str());
        double close = atof(closeStr.c_str());
        long volume = atol(volumeStr.c_str());
        history->append(dateStr, open, high, low, close, volume);
        rowsParsed++;
    }

    file.close();

    cout << "Loaded " << rowsParsed << " rows";
    if (rowsSkipped > 0) cout << " (" << rowsSkipped << " skipped)";
    cout << "\n";

    return history;
}

bool CSVParser::dateInRange(const string& date, const string& start, const string& end) {
    //check the if the date matches the range given
    return date >= start && date <= end;
}

int CSVParser::extractYear(const std::string& date) {
    //only 0-3 in YYYY-MM-DD
    if (date.size() < 4) return -1;
    //check if valid number
    for (int i = 0; i < 4; i++) if (!isdigit(date[i])) return -1;
    //return proper format 
    return (date[0] - '0') * 1000 + (date[1] - '0') * 100 + (date[2] - '0') * 10 +(date[3] - '0');
}

int CSVParser::extractMonth(const std::string& date) {
    //approximately check if it generally in the correct format - should only go up to 6 with YYYY-MM
    if (date.size() < 7) return -1;
    //check if valid numbers
    if (!isdigit(date[5]) || !isdigit(date[6])) return -1;
    //return proper format
    return (date[5] - '0') * 10 + (date[6] - '0');
}