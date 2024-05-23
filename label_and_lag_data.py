import pandas as pd
import numpy as np
from datetime import timedelta
import sys


def generate_labeled_data(date_str, input_file, output_file, lag_microseconds=5, noise_std=0.1):
    # Load raw data
    df = pd.read_csv(input_file)

    # Convert 'Time' column to datetime
    df['Time'] = pd.to_datetime(df['Time'])

    # Filter data for the specific date
    df = df[df['Time'].dt.strftime('%Y-%m-%d') == date_str]

    # Add an Exchange column, default to 'A'
    df['Exchange'] = 'A'

    # Randomly select 50% of the trades to be from Exchange B
    num_b_trades = len(df) // 2
    b_indices = np.random.choice(df.index, size=num_b_trades, replace=False)
    df.loc[b_indices, 'Exchange'] = 'B'

    # Add time lag and noise to Exchange B trades
    df_b = df.loc[b_indices].copy()
    df_b['Time'] += pd.to_timedelta(lag_microseconds, unit='us')
    df_b['Time'] += pd.to_timedelta(np.random.normal(0,
                                    noise_std, num_b_trades), unit='us')

    # Update the original DataFrame
    df.update(df_b)

    # Save the labeled data
    df.to_csv(output_file, index=False)

    print(f"Labeled data for {date_str} saved to {output_file}")


# Example usage
if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python label_and_lag_data.py <date>")
        sys.exit(1)

    date_str = sys.argv[1]
    input_file = "goog_trade_2.txt"
    output_file = f"labeled_trade_data_{date_str}.csv"

    generate_labeled_data(date_str, input_file, output_file)
