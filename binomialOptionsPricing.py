import numpy as np
import math
import matplotlib.pyplot as plt

N = 4
S0  = 100
T = 0.5
sigma = 0.4
dt = T/N
K =105
r = 0.05
u = np.exp( sigma * np.sqrt(dt) )
d =  np.exp( -sigma * np.sqrt(dt) )
p = ( np.exp(r*dt) - d) / (u -d)

for k in reversed(range(N+1)):
    ST = S0 * u**k * d ** (N-k)
    print(round(ST,2), round(max(ST-K,0),2))

def combos(n, i):
    return math.factorial(n) / (math.factorial(n-i)*math.factorial(i))
    
for k in reversed(range(N+1)):
    p_star = combos(N, k)*p**k *(1-p)**(N-k)
    print(round(p_star,2))

C=0   
for k in reversed(range(N+1)):
    p_star = combos(N, k)*p**k *(1-p)**(N-k)
    ST = S0 * u**k * d ** (N-k)
    C += max(ST-K,0)*p_star
    
print(np.exp(-r*T)*C)

N =4
S0  = 100
T = 0.5
sigma = 0.4
K = 105
r = 0.05

def binom_EU1(S0, K , T, r, sigma, N, type_ = 'call'):
    dt = T/N
    u = np.exp(sigma * np.sqrt(dt))
    d = np.exp(-sigma * np.sqrt(dt))
    p = (  np.exp(r*dt) - d )  /  (  u - d )
    value = 0 
    for i in range(N+1):
        node_prob = combos(N, i)*p**i*(1-p)**(N-i)
        ST = S0*(u)**i*(d)**(N-i)
        if type_ == 'call':
            value += max(ST-K,0) * node_prob
        elif type_ == 'put':
            value += max(K-ST, 0)*node_prob
        else:
            raise ValueError("type_ must be 'call' or 'put'" )
    
    print(value*np.exp(-r*T))
    return value*np.exp(-r*T)

print("testing {binom_EU1} function... Please hold")
print("\n\n\n")


binom_EU1(S0, K, T, r,sigma, N)



N=100000
sigma = 0.4
T = 0.5
K = 105
r= 0.05
dt = T / N
Heads = np.exp(sigma * np.sqrt(dt))
Tails = np.exp(-sigma * np.sqrt(dt))
S0 = 100
p = (  np.exp(r*dt) - Tails )  /  ( Heads - Tails )
paths = np.random.choice([Heads,Tails],p=[p,1-p],size=(N,1))
plt.plot(paths.cumprod(axis=0)*100, color='black');
plt.xlabel('Steps')
plt.ylabel('Stock Price')