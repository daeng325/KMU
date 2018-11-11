###########################################활성함수 ####################################

#파이썬2, 파이썬3 지원
from __future__ import division, print_function, unicode_literals

#관련 라이브러리
import os
import numpy as np
#%matplotlib inline
import matplotlib
import matplotlib.pyplot as plt

def logit(z):
    return 1/(1+np.exp(-z)) #로지스틱 시그모이드 활성함수

def relu(z):
    return np.maximum(0,z) #렉티파이어(ReLU)

def derivative(f, z, eps = 0.000001):
    return (f(z+eps)- f(z-eps)) / (2 * eps) #도함수 구하는 공식

z = np.linspace(-5, 5, 200)

plt.figure(figsize=(11,4))

plt.subplot(121)
plt.plot(z, np.sign(z), "r-", linewidth=2, label="step")
plt.plot(z, logit(z), "g--", linewidth=2, label="sigmoid")
plt.plot(z, np.tanh(z), "b-", linewidth=2, label="tanh")
plt.plot(z, relu(z), "m-", linewidth=2, label="ReLU")
plt.grid(True)
plt.legend(loc="center right", fontsize=14)
plt.title("activation function : g(z)", fontsize=14)
plt.axis([-5,5,-1.2,1.2])

plt.subplot(122)
plt.plot(z, derivative(np.sign,z), "r-", linewidth=2, label="step")
plt.plot(0, 0, "ro", markersize=5)
plt.plot(0, 0, "rx", markersize=10)
plt.plot(z, derivative(logit, z), "g--", linewidth=2, label="sigmoid")
plt.plot(z, derivative(np.tanh, z), "b-", linewidth=2, label="tanh")
plt.plot(z, derivative(relu,z), "m-", linewidth=2, label="ReLU")
plt.grid(True)
plt.title("gradient : g'(z)", fontsize=14)
plt.axis([-5, 5, -0.2, 1.2])

plt.show()

