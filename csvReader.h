#ifndef CSV_READER_H
#define CSV_READER_H

#include <fstream>
#include <string>
#include <vector>

class CSVReader {
public:
    CSVReader(const std::string &filename);

    // Read the next line from the CSV file. Returns true if successful, false if at the end of the file.
    bool readNext(std::vector<std::string> &row);

    // Check if the file is open.
    bool isOpen() const;

    // Close the CSV file.
    void close();

private:
    std::ifstream file;
    char delimiter; // You can specify a custom delimiter if needed.
};


CSVReader::CSVReader(const std::string &filename) {
    file.open(filename);
    delimiter = ','; // Default delimiter is a comma. You can change it as needed.
}

bool CSVReader::readNext(std::vector<std::string> &row) {
    row.clear();
    std::string line;

    if (std::getline(file, line)) {
        size_t pos = 0;
        size_t nextPos;
        while ((nextPos = line.find(delimiter, pos)) != std::string::npos) {
            row.push_back(line.substr(pos, nextPos - pos));
            pos = nextPos + 1;
        }
        row.push_back(line.substr(pos));
        return true;
    }

    return false; // End of file
}

bool CSVReader::isOpen() const {
    return file.is_open();
}

void CSVReader::close() {
    if (file.is_open()) {
        file.close();
    }
}

#endif // CSV_READER_H
