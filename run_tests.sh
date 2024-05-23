#!/bin/bash

# Compile the C++ program
g++ -o trade_analysis trade_analysis.cpp -std=c++11

# Array of test dates
dates=("2017-04-18" "2017-04-19" "2017-04-20")

# Expected results
expected_results=("5" "10" "15")

# Run the tests
for i in "${!dates[@]}"; do
    date="${dates[$i]}"
    expected="${expected_results[$i]}"
    echo "Running test for $date"
    ./trade_analysis "$date" > output.txt
    cat output.txt # Print the output for debugging
    result=$(awk -F' ' '/Estimation - B Exchange is/ {print $5}' output.txt)
    echo "Extracted result: $result" # Print the extracted result for debugging
    if [[ "$result" == "$expected" ]]; then
        echo "Test for $date passed! Expected: $expected, Got: $result"
    else
        echo "Test for $date failed! Expected: $expected, Got: $result"
    fi
done

# Clean up
rm output.txt
