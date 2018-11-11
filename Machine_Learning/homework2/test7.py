#####################################신경망 학습4 ####################
import torch

'''
nn 패키지는 신경망 레이어와 거의 동일한 모듈(Module)의 집합을 정의한다. 
모듈은 입력텐서를 받아 출력텐서를 계산하거나, 
학습가능한 파라미터를 포함하는 텐서와 같은 상태를 갖는다. 
또한, nn 패키지는 신경망을 학습할 때 일반적으로 사용하는 
유용한 손실 함수(loss function)을 정의한다.
'''

## 사용자 정의 모듈
class TwoLayerNet(torch.nn.Module):
    def __init__(self,D_in, H, D_out):
        super(TwoLayerNet,self).__init__()
        self.linear1 = torch.nn.Linear(D_in, H) #w1 ?
        self.linear2 = torch.nn.Linear(H, D_out) #w2 ?

    def forward(self,x):
        h_relu = self.linear1(x).clamp(min=0)
        y_pred = self.linear2(h_relu)
        return y_pred


N, D_in, H, D_out = 64, 1000, 100, 10

x = torch.randn(N,D_in)
y = torch.randn(N, D_out)

model = TwoLayerNet(D_in, H, D_out)

# 최적화
optimizer = torch.optim.SGD(model.parameters(), lr=1e-4)

for t in range(500):
    y_pred = model(x)
    loss = torch.nn.functional.mse_loss(y_pred, y)
    #torch.nn.MSELoss

    loss.backward()
    optimizer.step()
    optimizer.zero_grad()
