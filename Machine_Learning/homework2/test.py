import numpy as np

def logit(z):
    return 1/(1+np.exp(-z)) #로지스틱 시그모이드 활성함수
def relu(z):
    return np.maximum(0,z) #렉티파이어(ReLU)

x1 = 1
x2 = 0

o1 = -0.3 + x1 *1.0 +  x2*1.2
o2 = 1.6 + x1*-1.0 + x2*-1.1

x1 = relu(o1)
x2 = relu(o2)

o1 = 1.0 + x1*1.0 + x2*-1.0
o2 = 0.7 + x1*0.5 + x2*1.0

x1 = relu(o1)
x2 = relu(o2)

o1 = 0.5 + x1*-0.8 + x2*0.9
o2 = -0.1 + x1 * 0.3 + x2 * 0.4

x1 = relu(o1)
x2 = relu(o2)

o1 = 1.0 + x1*0.1 + x2*-0.2
o2 = -0.2 + x1*1.3 + x2*-0.4

result_o1 = relu(o1)
result_o2 = relu(o2)

print(result_o1)
print(result_o2)