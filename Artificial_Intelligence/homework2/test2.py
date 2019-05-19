#############################################오류 역전파 ################################

#파이썬2, 파이썬3 지원
from __future__ import division, print_function, unicode_literals

#관련 라이브러리
import os
import numpy as np
#%matplotlib inline
import matplotlib
import matplotlib.pyplot as plt

import numpy as np
np.random.seed(0)

N,D = 3,4

x= np.random.randn(N,D)#정규분포에서 추출
y = np.random.randn(N,D)
z = np.random.randn(N,D)

a = x * y
b = a + z
c = np.sum(b) #b행렬의 원소를 다 더한 값.

grad_c = 1.0
grad_b = grad_c * np.ones((N,D))
grad_a = grad_b.copy()
grad_z = grad_b.copy()
grad_x = grad_a * y
grad_y = grad_a * x

print("grad_c :\n", grad_c)
print("grad_b :\n", grad_b)
print("grad_a :\n", grad_a)
print("grad_z :\n", grad_z)
print("grad_x :\n", grad_x)
print("grad_y :\n", grad_y)


import torch
x = torch.randn(N, D, requires_grad = True) #계산이 마지막 레이어에 도달할 때까지 중간버퍼가 저장되지 않는다? ?? ???
y = torch.randn(N, D, requires_grad = True) #모든 연산에 대해 추적을 시작한다?
z = torch. randn(N, D)

a = x * y
b = a + z
c = torch.sum(b)

c.backward() #모든 gradient를 자동으로 계산해준다.

print("grad_x by torch :\n", x.grad)
print("grad_y by torch :\n", y.grad)