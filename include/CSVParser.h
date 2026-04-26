#ifndef CSVPARSER_H
#define CSVPARSER_H

#include <string>
#include "PriceHistory.h"

using std::string;

class CSVParser {
public:
    //parses the CSV file and returns a PriceHistory object containing the data
    static PriceHistory* loadHistory(const string& filename);

    //extraction helper functions for date parsing
    static bool dateInRange(const string& date, const string& start, const string& end);
    static int extractYear(const string& date);
    static int extractMonth(const string& date);
};

#endif