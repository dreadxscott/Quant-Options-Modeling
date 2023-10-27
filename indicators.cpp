#include "csvReader.h"
#include <cmath>
#include <deque>
#include <iostream>
#include <cstdlib>
#include <random>
#include <fstream>
#include <string>
#include <numeric>

using namespace std;

//A simple moving average, calculates the average of closing prices over a certain period
class SimpleMovingAverage{
public:
    
    deque<double> prices;
    deque<double> averages;
    double avg;
    int period;

    //this will construct the SMA with a specific period and
    //the name of a data file to open, read through and process
    SimpleMovingAverage(int per, const string& filename) {
        period = per;
        CSVReader reader(filename);

        if (!reader.isOpen()) {
            cerr << "Failed to open the CSV file." << endl;
            return;
        }

        vector<string> row;
        reader.readNext(row); //get rid of the header

        //starts up the prices
        for (int i = 0; i < period; i++) {

            reader.readNext(row);
            double newprice = std::stod(row[3]);
            prices.push_back(newprice);
    
        }
        
        get_average(); //first average gets calculated

        //Now, while we can still read the file, let's do our update process:
        while (reader.readNext(row)) {
            double newprice = std::stod(row[3]);
            update_prices(newprice);
            get_average();
        }

        reader.close();

    }

    void get_average() {
        double sum = 0;

        for(int i = 0; i < prices.size(); i++) {
            sum += prices.at(i);
        }

        avg = sum / period;
        averages.push_back(avg);
    }

    void update_prices(double new_price) {
        prices.pop_front();
        prices.push_back(new_price);
    }
};

//a moving average that weights more recent prices over past prices
class ExponentialMovingAverage {
public: 

    deque<double> prices;
    deque<double> averages;
    double avg;
    int period;
    double smoothing_const;


    ExponentialMovingAverage(int per, const std::string &filename) {
        period = per;
        smoothing_const = 2.0 / (period + 1);

        CSVReader reader(filename);

        if (!reader.isOpen()) {
            cerr << "Failed to open the CSV file." << endl;
            return;
        }

        vector<string> row;
        reader.readNext(row); //get rid of the header

        //starts up the prices
        for (int i = 0; i < period; i++) {

            reader.readNext(row);
            double newprice = std::stod(row[3]);
            prices.push_back(newprice);
        }
        
        avg = reduce(prices.begin(),prices.end(),0)/(double)prices.size();
        averages.push_back(avg);
        reader.close();
    }

    void update_prices(double new_price) {
        prices.pop_front();
        prices.push_back(new_price);
    }

    double get_average() {
        //the exponential average calculated after the first simple moving average
        //or the previous day's EMA, either of which will be stored in avg
        avg = prices.back() * smoothing_const + avg * (1 - smoothing_const);
        averages.push_back(avg);

        return avg;
    }

    void runEMA(const std::string &filename) {
        //Now, while we can still read the file, let's do our update process:
        CSVReader reader(filename);
        vector<string> row;

        if (!reader.isOpen()) {
            cerr << "Failed to open the CSV file." << endl;
            return;
        }

        //read the header and the first {period} number of lines
        for (int i = 0; i < period + 1; i++) {
            reader.readNext(row);
            cout << "Discarded row" << i+1 << endl;
        }

        
        while(reader.readNext(row)) {
            reader.readNext(row); //get the next row
            double newprice = std::stod(row[3]); //create a new price double
            update_prices(newprice); //add this price to the prices deque
            get_average(); //get the new average
            cout << "New average: " << avg << endl;
        }
    }
};

//an adaptive moving average that factors in the direction of the market and the market noise
//aka volatility
class KaufmanAdaptiveMovingAverage {
public:

    deque<double> prices;
    deque<double> averages;
    double avg;
    int period;
    double smoothing_const;
    double efficiency_ratio;


    //KAMA_i = KAMA_{i-1} + sc * (price - KAMA_{i-1})
    //this is the basic formula used for the calculation
    //of Kaufmann Adaptive Moving Average

    //needs two EMAs, one with a period of 2, and one with
    //a period of 30

    //Default Constructor, will need a constructor with 
    //methods for handling data input

    KaufmanAdaptiveMovingAverage(int per, const std::string &filename) {
        period = per;

        CSVReader reader(filename);

        if (!reader.isOpen()) {
            cerr << "Failed to open the CSV file." << endl;
            return;
        }

        vector<string> row;
        reader.readNext(row); //get rid of the header

        //starts up the prices
        for (int i = 0; i < period; i++) {

            reader.readNext(row);
            double newprice = std::stod(row[3]);
            prices.push_back(newprice);
    
        }

        get_efficiency_ratio();
        get_smoothing_const();
        
        double sum = 0;
        for (int i = 0; i < period; i++) {
            sum += prices.at(i);
        }
        avg = sum / period;

        //Now, while we can still read the file, let's do our update process:
        while (reader.readNext(row)) {
            double newprice = std::stod(row[3]);
            update_prices(newprice);
            get_efficiency_ratio();
            get_smoothing_const();
            get_average();
        }

        reader.close();
    }

    void get_efficiency_ratio() {
        double change;
        double volatility = 0;

        change = prices.back() - prices.front();
        
        for (int i = prices.size()-1; i != 1; i--) {
            volatility += abs(prices.at(i) - prices.at(i-1));
        }

        efficiency_ratio = change/volatility;
    }

    void get_smoothing_const() {
        smoothing_const = pow((efficiency_ratio*(2.0/3 - 2.0/31) + 2.0/31),2);
    }

    void get_average() {
        //KAMA_i = KAMA_{i-1} + sc * (price - KAMA_{i-1})
        avg = avg + smoothing_const * (prices.at(period-1) - avg);
        averages.push_back(avg);
    }

    void update_prices(double new_price) {
        prices.pop_front();
        prices.push_back(new_price);
    }
};


class BollingerBands {
public:

    deque<double> prices;
    deque<double> averages;
    double avg;
    int period;
    deque<double> std_devs;
    deque<double> upper_band;
    deque<double> lower_band;

    BollingerBands(int per, const std::string &filename) {
        period = per;
        CSVReader reader(filename);

        if (!reader.isOpen()) {
            cerr << "Failed to open the CSV file." << endl;
            return;
        }

        vector<string> row;
        reader.readNext(row); //get rid of the header

        //starts up the prices
        for (int i = 0; i < period; i++) {

            reader.readNext(row);
            double newprice = (std::stod(row[3]) + std::stod(row[4]) + std::stod(row[5])) / 3.0;
            prices.push_back(newprice);
    
        }
        
        get_average(); //first average gets calculated

        //Now, while we can still read the file, let's do our update process:
        while (reader.readNext(row)) {
            double newprice = (std::stod(row[3]) + std::stod(row[4]) + std::stod(row[5])) / 3.0;
            update_prices(newprice);
            get_average();
            get_bands();
        }

        reader.close();
    }

    double standard_deviation(); //maybe later I can add other options for price channels

    void get_bands() {
        double b1, b2;

        double std_dev = standard_deviation();
        b1 = avg + 2.0*std_dev; //upper band
        b2 = avg - 2.0*std_dev; //lower band

        std_devs.push_back(std_dev);
        upper_band.push_back(b1); //add to list of upper bands
        lower_band.push_back(b2); //add to list of lower bands
    }

        void get_average() {
        double sum = 0;

        for(int i = 0; i < prices.size(); i++) {
            sum += prices.at(i);
        }

        avg = sum / period;
        averages.push_back(avg);
    }

    void update_prices(double new_price) {
        prices.pop_front();
        prices.push_back(new_price);
    }

};

double BollingerBands::standard_deviation() {
    if (averages.empty()) {
        // Handle the case when the deque is empty.
        // You can choose to return a special value or throw an exception.
        // Here, we'll return NaN.
        return std::numeric_limits<double>::quiet_NaN();
    }

    double sum = 0.0;
    for (double price : prices) {
        sum += price;
    }

    double mean = sum / prices.size();

    double squaredDifferencesSum = 0.0;
    for (double price : prices) {
        double difference = price - mean;
        squaredDifferencesSum += difference * difference;
    }

    double variance = squaredDifferencesSum / prices.size();
    double stdDeviation = std::sqrt(variance);

    std_devs.push_back(stdDeviation);
    return stdDeviation;
}


class RelativeStrengthIndex {
public:
    int period = 14;
    double support_price;
    double resistance_price;
    deque<double> prices;
    deque<double> RSI;

    bool over_bought; //when RSI is above 70/100
    bool over_sold; //when RSI is below 30/100

    // RSI = 100 – (100 / [1 + {14-Day Average Gain / 14-Day Average Loss}])
    RelativeStrengthIndex(const std::string &filename) {
        CSVReader reader(filename);

        if (!reader.isOpen()) {
            cerr << "Failed to open the CSV file." << endl;
            return;
        }

        vector<string> row;
        reader.readNext(row); //get rid of the header

        //starts up the prices
        for (int i = 0; i < period; i++) {

            reader.readNext(row);
            double newprice = std::stod(row[3]);
            prices.push_back(newprice);
    
        }

        get_relative_strength();

        while (reader.readNext(row)) {
            double newprice = std::stod(row[3]);
            update_prices(newprice);
            get_relative_strength();
            cout << "Price is currently " << prices.back() <<
            " and the relative strength of the security is: "
            << RSI.back() << endl;
        }

    }

    void get_relative_strength() {
        double relative_strength;
        vector<double> wins, losses;

        for(int i = 0; i < (int)prices.size()-1; i++) {
            double ratio = prices.at(i+1) / prices.at(i); 
            ratio = ratio - 1;
            ratio = ratio * 100;

            if (ratio < 0) {
                losses.push_back(abs(ratio));
            }
            else if (ratio > 0) {
                wins.push_back(ratio);
            }    
        }

        double avg_win;
        double avg_loss;

        if (!wins.empty()) {
            avg_win = (double)reduce(wins.begin(), wins.end()) / wins.size();
        }
    
        if (!losses.empty()) {
            avg_loss = (double)reduce(losses.begin(), losses.end()) / losses.size();
        }

        if (avg_loss == 0) {
        // Handle division by zero here if needed
        // You can set a default value or skip RSI calculation
        relative_strength = 100.0;
        }

        else {
            relative_strength = avg_win / avg_loss;
            relative_strength = 100.0 - 100.0 / (1 + relative_strength);
        }

        RSI.push_back(relative_strength);
    }
    
    void update_prices(double new_price) {
        prices.pop_front();
        prices.push_back(new_price);
    }

    void set_flags() {

        if (RSI.back() >= 70) {
            over_bought = true;
            over_sold = false;
        }

        else if (RSI.back() <= 30) {
            over_bought = false;
            over_sold = true;
        }
    }

    void support_or_resistance() {
        if (RSI.back() > 50 && RSI.back() < 60 ) {
            resistance_price = prices.back();
        }

        else if (RSI.back() > 40 && RSI.back() < 50) {
            support_price = prices.back();
        }
    }

};

class MovingAverageConvergenceDivergence {
private:
    ExponentialMovingAverage* longEMA;
    ExponentialMovingAverage* shortEMA;

    int short_period = 12; //a 12-Day EMA
    int long_period = 26;  //a 26-day EMA
    int signal_period = 9; //a signal line for comparison    

public:

    bool sell_flag;
    bool buy_flag;

    double ema_diff;
    double hist;
    deque<double> signal;
    deque<double> MACD;
    deque<double> histogram;

    MovingAverageConvergenceDivergence(const string &filename) {
        CSVReader reader_short(filename);
        CSVReader reader_long(filename);
            
        if (!reader_long.isOpen() || !reader_short.isOpen()) {
            cerr << "Failed to open CSV reader." << endl;
            return;
        }

        longEMA = new ExponentialMovingAverage(long_period, filename); //instantiate long term EMA
        shortEMA = new ExponentialMovingAverage(short_period, filename); //instantiate short term EMA

        vector<string> row_long, row_short;

        //read off the headers
        reader_long.readNext(row_long); 
        reader_short.readNext(row_short);

        //catch up to the instantiated EMA places
        int i,j;
        for (i = 0; i < long_period; i++) {
            reader_long.readNext(row_long);
        }

        for (j = 0; j < short_period; j++) {
            reader_short.readNext(row_short);
        }

        //MACD while loop to get MACD long enough to create signal
        for (int k = 0; k < signal_period; k++) {
            double newprice_long = std::stod(row_long[3]);
            double newprice_short = std::stod(row_short[3]);

            longEMA->update_prices(newprice_long);
            longEMA->get_average();
            shortEMA->update_prices(newprice_short);
            shortEMA->get_average();

            ema_diff = shortEMA->averages.back() - longEMA->averages.back();
            MACD.push_back(ema_diff);
        }

        get_signal();

         while(reader_long.readNext(row_long)) {
            double newprice_long = std::stod(row_long[3]);
            double newprice_short = std::stod(row_short[3]);

            longEMA->update_prices(newprice_long);
            longEMA->get_average();
            shortEMA->update_prices(newprice_short);
            shortEMA->get_average();

            get_MACD();
            get_signal();
            set_flag();
         }
    }

    void get_MACD(){
        MACD.pop_front();
        ema_diff = shortEMA->averages.back() - longEMA->averages.back();
        MACD.push_back(ema_diff);
    }

    void get_signal() {
        if (MACD.size() != signal_period) {
            cerr << "MACD improperly made, adjust settings." << endl;
            return;
        }
        double sig;
        sig = reduce(MACD.begin(), MACD.end(), 0) / (double)MACD.size();
        signal.push_back(sig);

        hist = MACD.back() - signal.back();
        histogram.push_back(hist);
    }


    void set_flag(){
        if ((MACD.back()) > signal.back()){
            sell_flag = false;
            buy_flag = true;
            cout << "Buy flag triggered at " << signal.back() 
            << endl;
        }
        else if (MACD.back() < signal.back()) {
            sell_flag = true;
            buy_flag = false;
            cout << "Sell flag triggered at " << signal.back()
            << endl;    
        }
    }
};

class FibonacciRetracement {
public:
    vector<double> FibonacciRetracementLevels;
    vector<double> fibonacci  = {0.236, 0.382, 0.50, 0.618, 0.782}//typical levels used
    FibonacciRetracement(double price1, double price2) {
        if (price1 > price2) {
            cerr << "Not a Retracement" << endl;
            return;
        }
        for (int i = 0; i < (int)FibonacciRetracementLevels.size(); i++) {
            double level = price2/price1;
            level = price2 - (price2 - price1)*fibonacci.at(i);
            FibonacciRetracementLevels.push_back(level);   
        }
    }
};

class FibonacciExtension {
public:
    vector<double> FibonacciExtensionLevels;
    vector<double> fibonacci = {};
    FibonacciExtensionLevels(double price1, double price2) {
        
    }
};

int main() {
    //SimpleMovingAverage(20, "AAPL_options_data.csv");
    //ExponentialMovingAverage *EMA = new ExponentialMovingAverage(20, "AAPL_options_data.csv");
    //EMA->runEMA("AAPL_options_data.csv");
    //KaufmanAdaptiveMovingAverage(20, "AAPL_options_data.csv");
    //BollingerBands(20, "AAPL_options_data.csv");
    //RelativeStrengthIndex("AAPL_options_data.csv");
    //MovingAverageConvergenceDivergence("AAPL_options_data.csv");
    return 0;
}