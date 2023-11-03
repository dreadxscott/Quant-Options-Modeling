import numpy as np
import pandas as pd
from sklearn.preprocessing import MinMaxScaler
from sklearn.metrics import mean_squared_error
import tensorflow as tf
import matplotlib.pyplot as plt

# load your time series data
filename = 'AAPL_stock_data_LSTM.csv'
data = pd.read_csv(filename, index_col='Date', parse_dates=True)

# normalize the date range to [0,1]
scaler = MinMaxScaler()
data['Close'] = scaler.fit_transform(data['Close'].values.reshape(-1, 1))

# split the data in to training and testing sets
train_size = int(len(data['Close']) * 0.8)
train_data, test_data = data['Close'].iloc[:train_size], data['Close'].iloc[train_size:]

#create sequences of inputs (X) and their corresponding outputs (y) 
def create_sequences(data, sequence_length):
    X, y = [],[]
    for i in range(len(data) - sequence_length):
        input_sequence = data[i:i+sequence_length]
        X.append(input_sequence) 

        output = data[i+sequence_length]
        y.append(output)
    return np.array(X), np.array(y)

sequence_length = 10
X_train, y_train = create_sequences(train_data, sequence_length)
X_test, y_test = create_sequences(test_data, sequence_length)

model = tf.keras.models.Sequential()
LSTM, Dense = tf.keras.layers.LSTM, tf.keras.layers.Dense

model.add(LSTM(50, activation='relu', input_shape=(sequence_length,1)))
model.add(Dense(1))

model.compile(optimizer='adam', loss='mean_squared_error')

model.fit(X_train, y_train, epochs=50, batch_size=32)

# Use the trained LSTM model to make predictions on the test data
predictions = model.predict(X_test)
predictions = scaler.inverse_transform(predictions)  # Invert the scaling

actual_data = scaler.inverse_transform(test_data.iloc[sequence_length:].values.reshape(-1, 1))

#Assess the model performance
mse = mean_squared_error(actual_data, predictions)
rmse = np.sqrt(mse)
print(f'Root Mean Squared Error: {rmse}')

#Visualize the data prediction
plt.figure(figsize=(12, 6))
plt.plot(test_data.index[sequence_length:], actual_data, label='Actual')
plt.plot(test_data.index[sequence_length:], predictions, label='Predicted', color='red')
plt.title('LSTM Predictions vs. Actual')
plt.xlabel('Date')
plt.ylabel('Stock Price')
plt.legend()
plt.show()