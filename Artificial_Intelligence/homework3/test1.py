import torch
from torch.autograd import Variable
import torch.nn as nn
import torch.nn.functional as F

#신경망을 정의한다.
class Net(nn.Module):
    def __init__(self):
        super(Net,self).__init__()
        # 1 input image channel, 6 output channels, 5x5 square convolution
        # kernel
        self.conv1 = nn.Conv2d(1,6,5) #여러 입력 평면으로 구성된 입력 신호에 2d 컨볼루션을 적용합니다.  //일괄처리 크기, 채널 수, 커널 사이즈, 보폭
        self.conv2 = nn.Conv2d(6,16,5) #in_channel, out_channel, kernel_size
        # an affine operation : y=Wx+b
        self.fc1 = nn.Linear(16*5*5, 120)
        self.fc2 = nn.Linear(120,84)
        self.fc3 = nn.Linear(84,10)
        # 들어오는 데이터에 선형 변환을 적용합니다. #out_features : 각 출력 샘플의 크기 #in_features : 각 입력 샘플의 크기
        # bias를 false로 두면, 레이어에서 추가 바이어스를 학습하지 않는다.
        # Linear 함수가 완전연결층을 의미하는 것인가?

    def forward(self,x):
        # Max pooling over a (2,2) window
        x = F.max_pool2d(F.relu(self.conv1(x)),(2,2)) # 여러 입력 평면으로 구성된 입력 신호에 2d max 풀링을 적용합니다.
        # if the size is a square you can only specify a single number
        x = F.max_pool2d(F.relu(self.conv2(x)),2)
        x = x.view(-1, self.num_flat_features(x))
        x = F.relu(self.fc1(x))
        x = F.relu(self.fc2(x))
        x = self.fc3(x)
        return x
    #forward함수만 정의하게 되면, backward함수는 autograd를 사용해 자동으로 정의된다.

    def num_flat_features(self,x):
        size = x.size()[1:] # all dimensions except the batch dimension
        num_features = 1
        for s in size:
            num_features *= s
        return num_features
    #모델의 학습 가능한 매개변수들은 net.parameters()에 의해 반환된다.


net = Net()
print(net)

#(1)
#(2)


# net.parameters()를 사용하여 정의된 신경망의 학습가능한 매개변수들을 확인할 수 있음
params = list(net.parameters())
print(len(params))
print(params[0].size()) # conv1's weight
#(3)


# 다음의 임의의 32*32 입력을 가정함
# forward의 입력은 autograd.Variable이고, 출력 또한 마찬가지이다.
# 이 신경망에 MNIST 데이터셋을 사용하기 위해 데이터셋의 이미지를 32*32크기로 변경해야 한다.
input = Variable(torch.randn(1,1,32,32))
out = net(input)
print(out)
#(4)


# 오류 역전파를 통해 gradient를 구하기 전에 모든가중치의 gradient 버퍼들을 초기화
net.zero_grad()
out.backward(torch.randn(1,10)) #랜덤값으로 역전파를 한다.


# 손실 함수 정의 및 임의의 값들에 대해서 오차 결과 확인
# nn 패키지는 많이 사용되는 손실함수들을 제공하며, 해당 예제는 단순한 MSE를 사용
output = net(input)
target = Variable(torch.randn(10)) # a dummy target, for example
target = target.view(1, -1) # make it the same shape as output
criterion = nn.MSELoss()

loss = criterion(output,target)
print(loss)
#(5)


# 앞에 코드에서 언급한 것과 같이 오류 역전파하기 전, gradient를 초기화해야 함
# backward() 수행 후 어떤 변화가 있는지 확인하고, 초기화의 필요성을 확인함
net.zero_grad() #zeroes the gradientbuffers of all parameters

print('conv1.bias.grad before backward')
print(net.conv1.bias.grad)
#(6)

loss.backward()

print('conv1.bias.grad after backward')
print(net.conv1.bias.grad)
#(7)


# 스토캐스틱 경사하강법((미래)가중치 = (현재)가중치 - 학습률 * gradient) 을 이용하여
# 가중치를 갱신하는 코드는 다음과 같다.
learning_rate = 0.01
for f in net.parameters():
    f.data.sub_(f.grad.data * learning_rate)


# 하지만 위 구현 코드보다 실제, torc.optim에서 구현되는 SDG, Adam, RMSProp 등을 사용함
# 오류 역전파에서 최적화하는 방법을 보인 예제 코드
import torch.optim as optim

# create your optimizer
optimizer = optim.SGD(net.parameters(), lr=0.01)

#in your traning loop :
optimizer.zero_grad() #zero the gradient buffers
output = net(input)
loss = criterion(output,target)
loss.backward()
optimizer.step() #Does the update

