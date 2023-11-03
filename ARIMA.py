"""
An ARIMA model is a class of statistical models for analyzing and forecasting time series data.
"""
import pandas as pd
from statsmodels.tsa.arima.model import ARIMA
from statsmodels.tsa.stattools import adfuller
from statsmodels.graphics.tsaplots import plot_acf, plot_pacf
import math
import matplotlib.pyplot as plt

stock  = "AAPL_stock_info.csv"

def get_prices(stock):
    #Load data
    data  = pd.read_csv(stock)

    #change date to datetime and set it as index
    data['Date'] = pd.to_datetime(data['Date'])
    data.set_index('Date', inplace = True)

    #Check for and rid missing data
    data = data.dropna()

    return data

data = get_prices(stock)

def test_stationarity(series):
    result = adfuller(series)
    p_value = result[1]

    if p_value <= 0.05:
        print("The data is stationary (p-value: {})".format(p_value))
    else:
        print("The data is non-stationary (p-value: {})".format(p_value))

def difference_series(series, order=1):
    if order == 0:
        return series
    else:
        diff_series = series - series.shift(1)
        diff_series.dropna(inplace=True)
        return difference_series(diff_series, order - 1)
    
apple_diff = difference_series(data['Open'])

test_stationarity(apple_diff)
"""
# Plot ACF and PACF
plot_acf(data['Open'])
plt.title('Autocorrelation Function (ACF)')
plt.show()

plot_pacf(data['Open'])
plt.title('Partial Autocorrelation Function (PACF)')
plt.show()
"""

#define the ARIMA model
p, d, q = 2, 1, 9
model = ARIMA(data['Open'], order=(p, d, q))

# Fit the model
model_fit = model.fit()

# Number of periods to forecast
forecast_periods = 30
forecast = model_fit.forecast(steps=forecast_periods)

# Create a date range for the forecasted periods
forecast_index = pd.date_range(start=data.index[-1], periods=forecast_periods)

# Create a DataFrame for the forecast
forecast_df = pd.DataFrame(forecast)
forecast_df.index = forecast_index


# Plot the original data and the forecast
plt.figure(figsize=(12, 6))
plt.plot(data['Open'], label='Original Data')
plt.plot(forecast_df, label='Forecast', color='red')
plt.title('Stock Price Forecast')
plt.xlabel('Date')
plt.ylabel('Stock Price')
plt.legend()
plt.show()