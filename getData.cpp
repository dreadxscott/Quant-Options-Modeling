#include <iostream>
#include <string>
#include <vector>
#include <yfinance/yfinance.h>

// Function to read stock information from yfinance
	std::vector<std::string> readStockInformation(const std::string& symbol) {
	    std::vector<std::string> stockInfo;
	 
	    try {
	        // Fetch stock information using yfinance library
	        yfinance::Stock stock(symbol);
	 
	        // Get stock information
	        std::string name = stock.getName();
	        std::string price = std::to_string(stock.getPrice());
	        std::string volume = std::to_string(stock.getVolume());
	 
	        // Add stock information to the vector
	        stockInfo.push_back("Name: " + name);
	        stockInfo.push_back("Price: " + price);
	        stockInfo.push_back("Volume: " + volume);
	    } catch (const std::exception& e) {
	        // Log the error
	        std::cerr << "Error: " << e.what() << std::endl;
	    }
	 
	    return stockInfo;
	}

    	int main() {
	    // Example usage
	    std::string symbol = "AAPL";
	    std::vector<std::string> stockInfo = readStockInformation(symbol);
	 
	    // Print stock information
	    for (const std::string& info : stockInfo) {
	        std::cout << info << std::endl;
	    }
	 
	    return 0;
	}