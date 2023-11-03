#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sat Oct 21 16:50:59 2023

@author: collinfrancis

OOP Implementation of Black-Scholes
"""

from scipy.stats import norm
import numpy as np
import matplotlib.pyplot as plt

class BsOption:
    def __init__(self, S, K, T, r, sigma, q=0):
        self.S = S
        self.K = K
        self.T = T
        self.r = r
        self.sigma = sigma
        self.q = q

    @staticmethod
    def N(x):
        return norm.cdf(x)
    
    @property
    def params(self):
        return {'S': self.S,
                'K': self.K,
                'T': self.T,
                'r': self.r,
                'sigma': self.sigma,
                'q': self.q
            }
    
    def d1(self):
        return (np.log(self.S/self.K) + (self.r - self.q + self.sigma**2/2)*self.T) / (self.sigma * np.sqrt(self.T))
    
    def d2(self):
        return self.d1() - self.sigma*np.sqrt(self.T)
    
    def _call_value(self):
        return self.S*np.exp(-self.q*self.T)*self.N(self.d1()) - self.K*np.exp(-self.r*self.T) * self.N(self.d2())
    
    def _put_value(self):
        return self.K*np.exp(-self.r*self.T) * self.N(-self.d2()) -self.S*np.exp(-self.q*self.T)*self.N(-self.d1())
    
    def price(self, type_ = 'C'):
        if type_ == 'C': 
            return self._call_value()
        if type_ == 'P':
            return self._put_value()
        if type_ == 'B':
            return {'call' : self._call_value(), 'put': self._put_value()}
        else:
            raise ValueError('Unrecognized type')
            
if __name__ == '__main__':
    K = 100
    r = 0.1
    T = 1
    sigma = 0.3
    S = 100
    
    print(BsOption(S, K, T, r, sigma).price('B'))
        