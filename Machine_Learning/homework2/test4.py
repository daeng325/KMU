##############################################신경망 학습 ################################
import torch
import matplotlib.pyplot as plt

N, D_in, H, D_out = 64, 1000, 100, 10

#N : 배치 크기 / D_in : 입력의 차원 / H : 은닉 계층의  차원 / D_out : 출력의 차원
x = torch.randn(N, D_in)
y = torch.randn(N, D_out)
#가중치 랜덤 초기화
w1 = torch.randn(D_in, H)
w2 = torch.randn(H, D_out)

learning_rate = 1e-6
loss_var=list(range(500))
for t in range(500):
    # forward. y(예측값) 계산
    h = x.mm(w1)
    h_relu = h.clamp(min=0)#.clamp로 ReLU함수(활성함수)를 구현할 수 있다. h_relu는 2계층 입력으로 들어간다.
    # 0보다 작은 건 다 0으로 바뀌고 나머지는 그 값 그대로 출력.
    y_pred = h_relu.mm(w2)
    # 손실(loss) 계산
    loss = (y_pred - y).pow(2).sum()

    loss_var[t] = loss

    # backprop
    grad_y_pred = 2.0 * (y_pred-y)
    grad_w2 = h_relu.t().mm(grad_y_pred)
    grad_h_relu = grad_y_pred.mm(w2.t())
    grad_h = grad_h_relu.clone()
    grad_h[h<0] = 0
    grad_w1 = x.t().mm(grad_h)

    # 가중치 갱신
    w1 -= learning_rate * grad_w1
    w2 -= learning_rate * grad_w2

# index = list(range(500))
# plt.plot(index[:],loss_var[:])
# plt.show()

print(loss_var,"\n")