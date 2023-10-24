#include <cmath>
#include <deque>
#include <iostream>
#include <cstdlib>
#include <random>

using namespace std;

//A simple moving average, calculates the average of closing prices over a certain period
class SimpleMovingAverage{
public:
    
    deque<double> prices;
    deque<double> averages;
    double avg;
    int period;

    //default constructor
    SimpleMovingAverage()
    {
        //set period
        period = 20;

        //generate some random numbers; just to test the functionality
        std::random_device rd;  // Initialize the random number generator with a hardware seed.
        std::mt19937 gen(rd());  // Mersenne Twister pseudo-random generator.
        std::uniform_real_distribution<double> distribution(2000.0, 3000.0);

        //Loop for period, adding prices to deque:
        for (int i = 0; i < period; i++) {
            double random_price = distribution(gen);
            prices.push_back(random_price);
        }

        avg = get_average();
    }

    double get_average() {
        double sum = 0;

        for(int i = 0; i < prices.size(); i++) {
            sum += prices.at(i);
        }

        avg = sum / period;
        averages.push_back(avg);

        return avg;
    }

    void update_prices(double new_price) {
        prices.pop_front();
        prices.push_back(new_price);
    }

    //need to determine how to feed it prices based on datastreams
    void get_prices() {
        pass;
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

    ExponentialMovingAverage() 
    {
        //set period
        period = 20;
        smoothing_const = 2.0/(1 + period);

        //generate some random numbers; just to test the functionality
        std::random_device rd;  // Initialize the random number generator with a hardware seed.
        std::mt19937 gen(rd());  // Mersenne Twister pseudo-random generator.
        std::uniform_real_distribution<double> distribution(2000.0, 3000.0);

        //Loop for period, adding prices to deque:
        for (int i = 0; i < period; i++) {
            double random_price = distribution(gen);
            prices.push_back(random_price);
        }

        //need a simple moving average to start
        double first_avg;
        double sum = 0;

        for(int i = 0; i < prices.size(); i++) {
            sum += prices.at(i);
        }

        first_avg = sum / period;
        avg = first_avg;

        averages.push_back(first_avg);

    }

    void update_prices(double new_price) {
        prices.pop_front();
        prices.push_back(new_price);
    }

    double get_average() {
        double sum = 0;

        for(int i = 0; i < prices.size(); i++) {
            sum += prices.at(i);
        }

        //the exponentail average calculated after the first simple moving average
        avg = prices.back() * smoothing_const + avg * (1 - smoothing_const);
        averages.push_back(avg);

        return avg;
    }

    //need to determine how to feed it prices based on datastreams
    void get_prices() {
        pass;
    }

};

//an adaptive moving average that factors in the direction of the market and the market noise
//aka volatility
class KaufmanAdaptiveMovingAverage {
private:
    //two exponential moving averages to move in the background
    ExponentialMovingAverage slowEMA();
    ExponentialMovingAverage fastEMA();


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
    KaufmanAdaptiveMovingAverage() {
        pass;
    }

    double get_efficiency_ratio() {
        double change;
        double volatility = 0;

        change = abs(prices.back() - prices.front());
        
        for (int i = prices.size()-1; i != 1; i--) {
            volatility += prices.at(i) - prices.at(i-1);
        }

        return change/volatility;
    }

    double get_smoothing_const() {
        return (efficiency_ratio * (30 -2) + 2)**2;
    }

    double get_average() {
        //KAMA_i = KAMA_{i-1} + sc * (price - KAMA_{i-1})
        avg = avg + smoothing_const * (prices.at(period-1) - avg);
    }

    //need to determine how to feed it prices based on datastreams
    void get_prices() {
        pass;
    }
};

class BollingerBands {
private:
    SimpleMovingAverage BolSMA(); //later I can add a template to use any kind of MA

public:
    //Default Constructor
    BollingerBands() {

    }

    deque<double> std_devs;

    double standard_deviation(); //maybe later I can add other options for price channels
    double getSMA() {
       return BolSMA.avg;
    }

};

double BollingerBands::standard_deviation() {
    if (BolSMA.averages.empty()) {
        // Handle the case when the deque is empty.
        // You can choose to return a special value or throw an exception.
        // Here, we'll return NaN.
        return std::numeric_limits<double>::quiet_NaN();
    }

    double sum = 0.0;
    for (double average : BolSMA.averages) {
        sum += averages;
    }

    double mean = sum / BolSMA.averages.size();

    double squaredDifferencesSum = 0.0;
    for (double average : BolSMA.averages) {
        double difference = average - mean;
        squaredDifferencesSum += difference * difference;
    }

    double variance = squaredDifferencesSum / BolSMA.averages.size();
    double stdDeviation = std::sqrt(variance);

    std_devs.push_back(stdDeviation);
    return stdDeviation;
}


int main() {
    SimpleMovingAverage(SMA);
    ExponentialMovingAverage(EMA);
    KaufmanAdaptiveMovingAverage(KAMA);
    BollingerBands(BB);
}