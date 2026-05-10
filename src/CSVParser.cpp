#include "CSVParser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <vector>

using std::string;
using std::ifstream;
using std::istringstream;
using std::getline;
using std::cerr;
using std::cout;
using std::vector;

//load the file data into price history linked list
PriceHistory* CSVParser::loadHistory(const string& filename) {
    //check and open file
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open file: " << filename << "\n";
        return nullptr;
    }

    //new price history to store the parsed data into
    PriceHistory* history = new PriceHistory();

    //for parsing the header and detecting column positions
    string line;

    //read the header
    if (!getline(file, line)) {
        cerr << "File is empty: " << filename << "\n";
        delete history;  return nullptr;
    }

    //examine the header files, since the order can differ per file
    vector<string> headers;
    istringstream headerStream(line);
    string column;

    //store the header columns in a vector for easy access and so we can check them 
    while (getline(headerStream, column, ',')) headers.push_back(column);

    //indices for the relevant columns - will be detected from the header
    int dateIdx   = -1, openIdx   = -1, highIdx   = -1, lowIdx    = -1, closeIdx  = -1,volumeIdx = -1;

    //column positions are detected from the header, so we can handle files with different column orders
    for (int i = 0; i < headers.size(); i++) {
        //check header column and assign the index 
        if (headers[i] == "Date") {
            dateIdx = i;
        } else if (headers[i] == "Open") {
            openIdx = i;
        } else if (headers[i] == "High") {
            highIdx = i;
        } else if (headers[i] == "Low") {
            lowIdx = i;
        } else if (headers[i] == "Close") {
            closeIdx = i;
        } else if (headers[i] == "Volume") {
            volumeIdx = i;
        } 
    }

    int rowsParsed = 0;
    int rowsSkipped = 0;

    while (getline(file, line)) {

        if (line.empty()) continue;

        // split row into columns
        vector<string> values;

        //split line by commas, store in values vector
        istringstream ss(line);
        string value;
        while (getline(ss, value, ',')) values.push_back(value); 

        //check if enough columns for all data in the file, otherwise skip since we cannot store
        if (values.size() < headers.size()) {
            rowsSkipped++;
            continue;
        }

        //extract the relevant data from the columns using the detected indices
        string dateStr = values[dateIdx];
        //use .c_str() to convert from string to const char* for atof and atol functions
        double open = atof(values[openIdx].c_str());
        double high = atof(values[highIdx].c_str());
        double low = atof(values[lowIdx].c_str());
        double close = atof(values[closeIdx].c_str());
        long volume = atol(values[volumeIdx].c_str());

        //append the data to the price history linked list
        history->append(dateStr, open, high, low, close, volume);

        //increment the count of successfully parsed rows
        rowsParsed++;
    }

    file.close();   //when finished exit the file

    cout << "Loaded " << rowsParsed << " rows"; //parsing statistic

    if (rowsSkipped > 0) cout << " (" << rowsSkipped << " skipped)";
    cout << "\n";

    //return the loaded price history
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