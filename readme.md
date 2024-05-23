# Trade Data Time Lag Estimation

This project estimates the time lag between trades from two exchanges (Exchange A and Exchange B) using synthetic trade data. Follow the steps below to generate test data, compile the C++ program, and run the tests.

## Setup

1. **Install Python Dependencies:**
    ```bash
    pip install pandas numpy
    ```

## Generate Test Data:

python generate_test_cases.py

## Compile the C++ Program:

g++ -o trade_analysis trade_analysis.cpp -std=c++11

## Run the Tests:

bash run_tests.sh
