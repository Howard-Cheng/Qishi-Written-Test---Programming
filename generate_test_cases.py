import pandas as pd
import numpy as np
from datetime import timedelta
import os


def generate_test_data(date_str, num_trades=10000, lag_microseconds=5, noise_std=0.1, folder="test_data"):
    """
    Generates synthetic trade data with realistic distribution and time lag.

    Args:
        date_str (str): Date string in the format 'YYYY-MM-DD'.
        num_trades (int): Number of trades to generate.
        lag_microseconds (int): Time lag in microseconds to add to trades from Exchange B.
        noise_std (float): Standard deviation of the noise to add to the timestamps.
        folder (str): Folder to save the generated data.

    Returns:
        None
    """
    if not os.path.exists(folder):
        os.makedirs(folder)

    # Generate timestamps
    timestamps = pd.date_range(
        start=f"{date_str} 09:30:00", periods=num_trades, freq="ms")
    tickers = ["XYZ"] * num_trades
    prices = np.random.uniform(low=100, high=200, size=num_trades)
    sizes = np.random.uniform(low=1, high=100, size=num_trades)

    # Create a DataFrame
    df = pd.DataFrame({
        "Time": timestamps,
        "Ticker": tickers,
        "Price": prices,
        "Size": sizes
    })

    # Use a probability-based method to assign exchanges
    exchange_prob = np.random.uniform(0, 1, num_trades)
    # Assume 60% trades in A, 40% in B
    df['Exchange'] = np.where(exchange_prob < 0.6, 'A', 'B')

    # Add time lag and noise to Exchange B trades
    b_indices = df[df['Exchange'] == 'B'].index
    df.loc[b_indices, "Time"] += pd.to_timedelta(lag_microseconds, unit="us")
    df.loc[b_indices, "Time"] += pd.to_timedelta(
        np.random.normal(0, noise_std, len(b_indices)), unit="us")

    # Save the labeled data
    output_file = f"{folder}/labeled_trade_data_{date_str}.csv"
    df.to_csv(output_file, index=False)

    print(
        f"Generated test data for {date_str} with a lag of {lag_microseconds} microseconds.")


# Generate test cases for different dates and lags
generate_test_data("2017-04-18", lag_microseconds=5)
generate_test_data("2017-04-19", lag_microseconds=10)
generate_test_data("2017-04-20", lag_microseconds=15)
