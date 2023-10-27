#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

int main() {
    std::ifstream file("AAPL_options_data.csv");
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<std::string> tokens;

        std::string token;
        while (std::getline(iss, token, ',')) {
            tokens.push_back(token);
        }

        // Now 'tokens' contains the fields of the CSV row
        for (const auto& field : tokens) {
            std::cout << field << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
