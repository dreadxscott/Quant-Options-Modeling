"""
This will be a program where we calculate not only the black-scholes model
but also the greeks (partial derivatives) of the black-scholes model
"""
import math
import numpy as np
from scipy.stats import norm
import matplotlib.pyplot as plt

# The inputs of the Black-Scholes equation are as follows:
# S = underlying price ($$$ per share) (the price at which you can buy or sell the option)
# K = strike price ($$$ per share)
# sigma =  volatility (percent per annum)
# r = continuously-compounded risk-free interest rate (percent per annum)
# q = comntinuously-compounded risk-free divident yield (percent per annum)
# t = time to expiration (percent of a year)

N  = norm.cdf

def BS_CALL(S, K, sigma, r, q, t):
    #this function will determine the Call Price based on the formula:
    d1 = (math.log(S/K) + t * (r - q + (sigma**2)/2))/ (sigma* math.sqrt(t))
    d2 = d1 - sigma*math.sqrt(t)
    call = S * math.exp(-q*t) * N(d1) - K * math.exp(-r*t) * N(d2)
    return call

def BS_PUTS(S, K, sigma, r, q, t):
    #this function will determine the Put Price based on the formula:
    d1 = (math.log(S/K) + t * (r - q + (sigma**2)/2))/ (sigma* math.sqrt(t))
    d2 = d1 - sigma*math.sqrt(t)
    put = K * math.exp(-r*t) * N(-d2) - S * math.exp(-q*t) * N(-d1)
    return put

N_prime = norm.pdf

def BS_DELTA_CALL(S, K, sigma, r, q, t): 
    d1 = (math.log(S/K) + t * (r - q + (sigma**2)/2))/ (sigma* math.sqrt(t))
    delta_c = math.exp(-q*t) * N(d1)
    return delta_c

def BS_DELTA_PUT(S, K, sigma, r, q, t):
    d1 = (math.log(S/K) + t * (r - q + (sigma**2)/2))/ (sigma* math.sqrt(t))
    delta_p = math.exp(-q*t) * (N(d1) -1)
    return delta_p

def BS_GAMMA(S, K, sigma, r, q, t):
    d1 = (math.log(S/K) + t * (r - q + (sigma**2)/2))/ (sigma* math.sqrt(t))
    gamma = (math.exp(-q*t) / S*sigma*math.sqrt(t)) * N_prime(d1)
    return gamma

def BS_THETA_CALL(S, K, sigma, r, q, t, T):
    d1 = (math.log(S/K) + t * (r - q + (sigma**2)/2))/ (sigma* math.sqrt(t))
    d2 = d1 - sigma*math.sqrt(t)
    theta_c = (1/T) * ((-1* ((S*sigma*math.exp(-q*t))/(2*math.sqrt(t)))*N_prime(d1)) - r*K*math.exp(-r*t)*N(d2) + q*S*math.exp(-q*t)*N(d1))
    return theta_c

def BS_THETA_PUT(S, K, sigma, r, q, t, T):
    d1 = (math.log(S/K) + t * (r - q + (sigma**2)/2))/ (sigma* math.sqrt(t))
    d2 = d1 - sigma*math.sqrt(t)
    theta_p = (1/T) * ((-1* ((S*sigma*math.exp(-q*t))/(2*math.sqrt(t)))*N_prime(d1)) + r*K*math.exp(-r*t)*N(-d2) - q*S*math.exp(-q*t)*N(-d1))
    return theta_p

def BS_VEGA(S, K, sigma, r, q, t):
    d1 = (math.log(S/K) + t * (r - q + (sigma**2)/2))/ (sigma* math.sqrt(t))
    vega = (1/100) * S * math.exp(-q*t) * math.sqrt(t) * N_prime(d1)
    return vega

def BS_RHO_CALL(S, K, sigma, r, q, t):
    d1 = (math.log(S/K) + t * (r - q + (sigma**2)/2))/ (sigma* math.sqrt(t))
    d2 = d1 - sigma*math.sqrt(t)
    rho_c = (1/100) * (K * t * math.exp(-r*t) * N(d2))
    return rho_c

def BS_RHO_PUT(S, K, sigma, r, q, t):
    d1 = (math.log(S/K) + t * (r - q + (sigma**2)/2))/ (sigma* math.sqrt(t))
    d2 = d1 - sigma*math.sqrt(t)
    rho_p = (-1/100) * (K * t * math.exp(-r*t) * N(-d2))
    return rho_p


