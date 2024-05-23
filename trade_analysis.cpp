#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <iomanip>
#include <algorithm>

// Struct to hold trade data
struct Trade {
    std::chrono::system_clock::time_point time;
    std::string ticker;
    double price;
    double size;
    std::string exchange;
};

// Function to parse a single line of CSV
Trade parseCSVLine(const std::string& line) {
    std::stringstream ss(line);
    std::string token;
    Trade trade;

    try {
        std::getline(ss, token, ',');
        std::tm tm = {};
        std::istringstream tokenStream(token);
        tokenStream >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        int microseconds = 0;
        size_t pos = token.find('.');
        if (pos != std::string::npos) {
            microseconds = std::stoi(token.substr(pos + 1));
            token = token.substr(0, pos);
            tokenStream.str(token);
            tokenStream.clear();
            tokenStream >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        }
        trade.time = std::chrono::system_clock::from_time_t(std::mktime(&tm)) + std::chrono::microseconds(microseconds);

        std::getline(ss, trade.ticker, ',');
        std::getline(ss, token, ',');
        trade.price = std::stod(token);
        std::getline(ss, token, ',');
        trade.size = std::stod(token);
        std::getline(ss, trade.exchange, ',');
    } catch (const std::exception& e) {
        std::cerr << "Error parsing line: " << line << " - " << e.what() << std::endl;
        trade.price = 0.0;
        trade.size = 0.0;
    }

    return trade;
}

// Function to read CSV data
std::vector<Trade> readCSV(const std::string& filePath) {
    /**
     * Reads trade data from a CSV file and stores it in a vector of Trade structs.
     *
     * Args:
     *     filePath (const std::string&): The path to the CSV file.
     *
     * Returns:
     *     std::vector<Trade>: A vector containing the trade data.
     */
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return {};
    }

    std::string line;
    std::vector<Trade> trades;

    // Skip the header line
    std::getline(file, line);

    while (std::getline(file, line)) {
        if (!line.empty()) {
            Trade trade = parseCSVLine(line);
            if (trade.price != 0.0 || trade.size != 0.0) { // Skip invalid trades
                trades.push_back(trade);
            }
        }
    }

    std::cout << "Read " << trades.size() << " trades from " << filePath << std::endl;

    return trades;
}

// Function to find the closest match in time for a given trade from another set of trades
Trade findClosestTrade(const Trade& trade, const std::vector<Trade>& trades) {
    /**
     * Finds the closest matching trade in time from another set of trades.
     *
     * Args:
     *     trade (const Trade&): The trade to find a match for.
     *     trades (const std::vector<Trade>&): The vector of trades to search.
     *
     * Returns:
     *     Trade: The closest matching trade.
     */
    auto closest = std::min_element(trades.begin(), trades.end(), [&trade](const Trade& a, const Trade& b) {
        return std::abs(std::chrono::duration_cast<std::chrono::microseconds>(a.time - trade.time).count()) <
               std::abs(std::chrono::duration_cast<std::chrono::microseconds>(b.time - trade.time).count());
    });
    return *closest;
}

// Function to calculate the average time difference between trades from Exchange A and Exchange B
int estimateTimeLag(const std::vector<Trade>& tradesA, const std::vector<Trade>& tradesB) {
    /**
     * Estimates the average time lag between trades from Exchange A and Exchange B.
     *
     * Args:
     *     tradesA (const std::vector<Trade>&): The vector of trades from Exchange A.
     *     tradesB (const std::vector<Trade>&): The vector of trades from Exchange B.
     *
     * Returns:
     *     int: The estimated average time lag in microseconds.
     */
    if (tradesA.empty() || tradesB.empty()) {
        std::cerr << "Error: Not enough trades from both exchanges to estimate lag." << std::endl;
        return 0;
    }

    // Calculate the average time difference
    int64_t totalDiff = 0;
    size_t count = 0;

    for (const auto& tradeA : tradesA) {
        Trade closestTradeB = findClosestTrade(tradeA, tradesB);
        auto diff = std::chrono::duration_cast<std::chrono::microseconds>(closestTradeB.time - tradeA.time).count();
        totalDiff += diff;
        count++;
    }

    int averageDiff = static_cast<int>(totalDiff / count);
    return averageDiff;
}

int main(int argc, char* argv[]) {
    /**
     * Main function to estimate the time lag between exchanges.
     *
     * Args:
     *     argc (int): Argument count.
     *     argv (char*[]): Argument values.
     *
     * Returns:
     *     int: Exit status.
     */
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_date>" << std::endl;
        return 1;
    }

    std::string date_str = argv[1];
    std::string inputFilePath = "test_data/labeled_trade_data_" + date_str + ".csv";

    // Step 1: Read data
    std::vector<Trade> trades = readCSV(inputFilePath);
    if (trades.empty()) {
        std::cerr << "No trades read from the file." << std::endl;
        return 1;
    }

    // Separate trades by exchange
    std::vector<Trade> tradesA, tradesB;
    for (const auto& trade : trades) {
        if (trade.exchange == "A") {
            tradesA.push_back(trade);
        } else {
            tradesB.push_back(trade);
        }
    }

    std::cout << "Trades from Exchange A: " << tradesA.size() << std::endl;
    std::cout << "Trades from Exchange B: " << tradesB.size() << std::endl;

    // Step 2: Estimate the time lag
    int estimatedLag = estimateTimeLag(tradesA, tradesB);
    std::cout << "Estimation - B Exchange is " << estimatedLag << " microseconds slower than A Exchange on " << date_str << std::endl;

    return 0;
}
