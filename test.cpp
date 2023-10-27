//test.cpp
//testing the csvReader.h

#include "csvReader.h"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    CSVReader reader("AAPL_options_data.csv");

    if (!reader.isOpen()) {
        cerr << "Failed to open the CSV file." << endl;
        return 1;
    }

    vector<string> row;
    reader.readNext(row); //get rid of the header
    
    //extracts the last price
    //so then with this, we'' 
    while (reader.readNext(row)) {
        cout << row[3] << " ";

        cout << endl; 
    }
    reader.close();

    return 0;
}