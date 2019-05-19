###########################################신경망 학습2 ################################
import torch
import matplotlib.pyplot as plt

N, D_in, H, D_out = 64, 1000, 100, 10

x = torch.randn(N, D_in)
y = torch.randn(N, D_out)
w1 = torch.randn(D_in, H, requires_grad=True)
w2 = torch.randn(H, D_out, requires_grad=True)

learning_rate = 1e-6
loss_var=list(range(500))
for t in range(500):
    y_pred = x.mm(w1).clamp(min=0).mm(w2)
    loss = (y_pred - y).pow(2).sum()
    loss_var[t] = loss

    # autograde를 사용하여 역전파 단계를 자동으로 계산해준다.
    loss.backward()

    #가중치 갱신
    with torch.no_grad():
        w1 -= learning_rate * w1.grad
        w2 -= learning_rate * w2.grad

        #가중치 갱신 후에는 수동으로 변화도를 0으로 만든다.
        w1.grad.zero_()
        w2.grad.zero_()

index = list(range(500))
plt.plot(index, loss_var)
plt.show()
