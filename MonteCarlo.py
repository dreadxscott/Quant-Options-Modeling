#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Oct 21 17:23:19 2023

@author: collinfrancis
"""
"""
Using the Monte Carlo method is one of the most popular ways to price options.
"""

import numpy as np
from scipy.stats import norm
import matplotlib.pyplot as plt

Norm = norm.cdf

def geo_paths(S, T, r, q, sigma, steps, N):
    """
    Inputs
    #S = current stock price
    #K = strike price
    #T = Time to maturity, one year = 1, 1 month = 1/12
    #r = risk free interest rate
    #q = dividend yield
    #sigma = volatility
    
    Outputs
    #[steps,N] Matrix of asset paths
    """
    
    dt = T/steps
    #S_{T} = ln(S_{0}) + \int_{0}^T(\mu-\frac{\sigma^2}{2})dt + \int_{0}^T\sigma dW(t)
#    ST = np.log(S) + np.cumsum(((r - q - sigma**2/2) * dt + sigma*np.sqrt(dt) * np.random.normal(size=(steps,N)))axis=0)
    ST = np.log(S) +  np.cumsum(((r - q - sigma**2/2)*dt +\
                              sigma*np.sqrt(dt) * \
                              np.random.normal(size=(steps,N))),axis=0)
    
    
    return np.exp(ST)

S = 100
K = 110
T = 1/2
r = 0.05
q = 0.02
sigma = 0.25
steps = 100
N = 100000

paths = geo_paths(S, T, r, q, sigma, steps, N)

plt.plot(paths)
plt.xlabel("Time Increments")
plt.ylabel("Stock Price")
plt.title("Geometric Brownian Motion")

def black_scholes_call(S, K, T, r, q, sigma):
    
    d1 = (np.log(S/K) + (r - q + sigma**2/2) * T) / ( sigma * np.sqrt(T))
    d2 = d1 - sigma * np.sqrt(T)
    
    
    bs_call = S * np.exp(-q*T) * Norm(d1) - K * np.exp(-r*T) * Norm(d2)
    
    # bs_call = S
    
    # bs_call *= np.exp(-q*T)
    
    # print(N) 
    
    # bs_call *= Norm(d1)
    
    # part2 = K
    
    # part2 *= np.exp(-r*T)
    
    # part2 *= Norm(d2)
    
    # bs_call -= part2
    
    return bs_call
    
payoffs = np.maximum(paths[-1]-K,0)
option_price = np.mean(payoffs)*np.exp(-r*T)

bs_price = black_scholes_call(S, K, T, q, r, sigma)

print(f"Black-Scholes price is {bs_price}")
print(f"Simulated price is {option_price}")


"""
so as you can see, by changing N, you get simulated price and Black-Scholes price closer to each other. the following is a visualization of what is happening
"""

# n, bins, patches = plt.hist(paths[-1],bins=250)
# for c, p in zip(bins,patches):
#     if c > K:
#         plt.step(p, 'facecolor', 'green')
#     else:
#         plt.step(p, 'facecolor', 'red')
        
# plt.axvline(K, color='black', linestyle='dashed', linewidth=2,label="Strike")
# plt.title("Distribution of $S_{T}$")
# plt.xlabel("$S_{T}$")
# plt.ylabel('Count')
# plt.legend()

n, bins, patches = plt.hist(paths[-1],bins=250);
for c, p in zip(bins, patches):
    if c > K:
        plt.setp(p, 'facecolor', 'green')
    else:
        plt.setp(p, 'facecolor', 'red')

plt.axvline(K, color='black', linestyle='dashed', linewidth=2,label="Strike")
plt.title("Distribution of $S_{T}$")
plt.xlabel("$S_{T}$")
plt.ylabel('Count')
plt.legend()

"""
Monte-Carlo might seem inaccurate, but it's good because sometimes a closed form solution isn't available 


"""