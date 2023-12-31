import yfinance as yf
import pandas as pd

def save_options_data_to_csv(stock_symbol, output_csv):
    try:
        # Create a Ticker object for the specified stock symbol
        ticker = yf.Ticker(stock_symbol)

        # Fetch options data
        options_data = ticker.options

        # Create an empty DataFrame to store options data
        options_df = pd.DataFrame()

        for option_date in options_data:
            option_chain = ticker.option_chain(option_date)
            option_chain_df = option_chain.calls  # Use calls data (you can also get puts data)
            option_chain_df['Expiration Date'] = option_date  # Add an expiration date column
            options_df = pd.concat([options_df, option_chain_df])

        # Save options data to a CSV file
        options_df.to_csv(output_csv, index=False)

        print(f"Options data for {stock_symbol} saved to {output_csv}")
    except Exception as e:
        print(f"An error occurred: {e}")

def save_stock_data_to_csv(stock_symbol, output_csv):
    try:
        # Create a Ticker object for the specified stock symbol
        ticker = yf.Ticker(stock_symbol)

        # Get historical data for the past month
        historical_data = ticker.history(period="12mo")

        # Save the historical data to a CSV file
        historical_data.to_csv(output_csv)

        print(f"Stock data for {stock_symbol} saved to {output_csv}")
    except Exception as e:
        print(f"An error occurred: {e}")

# Example usage:
stock_symbol = "AAPL"
save_stock_data_to_csv(stock_symbol, f"{stock_symbol}_stock_data_LSTM.csv")
#save_options_data_to_csv(stock_symbol, f"{stock_symbol}_options_data.csv")