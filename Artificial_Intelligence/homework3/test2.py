'''
분류기 학습은 다음과 같은 과정을 가짐
1. 정규화된 CIFAR-10 훈련집합과 테스트집합을 torchvision을 이용하여 적재함
2. 컨볼루션 신경망을 정의함
3. 손실함수 정의 / 교차 엔트로피와 SGD+ momentum
4. 훈련집합을 이용하여 신경망을 학습시킴
5. 테스트집합을 이용하여 신경망 성능 확인
'''

### 1.
from multiprocessing import Process, freeze_support
import torch
import torchvision
import torchvision.transforms as transforms


if __name__ == '__main__':
    freeze_support()
    # torchvision dataset의 출력은 [0,1] 범위를 갖는 PILImage라고 한다. 이를 [-1,1]범위로 정규화된 Tensor로 변환한다.

    transform = transforms.Compose([transforms.ToTensor(),
                                    transforms.Normalize((0.5, 0.5, 0.5), (0.5, 0.5, 0.5))])

    trainset = torchvision.datasets.CIFAR10(root='./data', train=True,
                                            download=True, transform=transform)
    trainloader = torch.utils.data.DataLoader(trainset, batch_size=4,
                                              shuffle=True, num_workers=2)

    testset = torchvision.datasets.CIFAR10(root='./data', train=False,
                                           download=True, transform=transform)
    testloader = torch.utils.data.DataLoader(testset, batch_size=4,
                                             shuffle=False, num_workers=2)

    classes = ('plane', 'car', 'bird', 'cat', 'deer', 'dog', 'frog', 'horse', 'ship', 'truck')
    # (1)

    # 훈련집합의 일부 사진들 확인
    import matplotlib.pyplot as plt
    import numpy as np


    # functions to show an image //이미지 보여주기 위한 함수이다.
    def imshow(img):
        img = img / 2 + 0.5  # unnormalize
        npimg = img.numpy()
        plt.imshow(np.transpose(npimg, (1, 2, 0)))
        plt.show()


    # get some random training images // 학습용 이미지를 무작위로 가져온다.
    dataiter = iter(trainloader)
    images, labels = dataiter.next()

    # show images
    imshow(torchvision.utils.make_grid(images))
    # print labels
    print(' '.join('%5s' % classes[labels[j]] for j in range(4)))
    # (2)

    ### 2.
    # 3채널 32*32 크기의 사진을 입력받고, 신경망을 통과해 10 부류를 수행
    from torch.autograd import Variable
    import torch.nn as nn
    import torch.nn.functional as F


    class Net(nn.Module):
        def __init__(self):
            super(Net, self).__init__()
            # 1 input image channerl, 6 output channels, 5x5 square convolution
            # kernel
            self.conv1 = nn.Conv2d(3, 6, 5)
            self.pool = nn.MaxPool2d(2, 2)
            self.conv2 = nn.Conv2d(6, 16, 5)
            # an affine operation : y=Wx+b
            self.fc1 = nn.Linear(16 * 5 * 5, 120)
            self.fc2 = nn.Linear(120, 84)
            self.fc3 = nn.Linear(84, 10)

        def forward(self, x):
            # Max pooling over a (2,2) window
            x = self.pool(F.relu(self.conv1(x)))
            x = self.pool(F.relu(self.conv2(x)))
            x = x.view(-1, 16 * 5 * 5)
            x = F.relu(self.fc1(x))
            x = F.relu(self.fc2(x))
            x = self.fc3(x)
            return x


    net = Net()

    ### 3.
    import torch.optim as optim

    criterion = nn.CrossEntropyLoss()
    optimizer = optim.SGD(net.parameters(), lr=0.001, momentum=0.9)

    ### 4. 훈련 집합으로 학습시킴.
    for epoch in range(2):  # loop over the dataset multiple times

        running_loss = 0.0
        for i, data in enumerate(trainloader, 0):
            # get the inputs
            inputs, labels = data

            # Variable
            inputs, labels = Variable(inputs), Variable(labels)

            # zero the parameter gradients
            optimizer.zero_grad()

            # forward + backward + optimize
            outputs = net(inputs)
            loss = criterion(outputs, labels)
            loss.backward()
            optimizer.step()

            # print statistics (통계 출력)
            running_loss += loss.data[0]
            if i % 1000 == 999:  # print every 1000 mini-batches
                print('[%d, %5d] loss : %.3f' %
                      (epoch + 1, i + 1, running_loss / 1000))
                running_loss = 0.0

    print('Finished Training')
    # (3)

    ### 5. 테스트집합으로 성능 확인
    dataiter = iter(testloader)
    images, labels = dataiter.next()

    # print images
    imshow(torchvision.utils.make_grid(images))
    print('GroundTruth:', ' '.join('%5s' % classes[labels[j]] for j in range(4)))
    # (4)

    outputs = net(Variable(images))
    # 출력은 10개 분류 각각에 대한 값으로 나타난다.
    # 어떤 분류에 대해 더 높은 값이 나타난다는 것은, 신경망이 그 이미지가 더 해당 분류에 가깝다고 생각한다는 것이다.
    # 가장 높은 값을 갖는 index를 뽑아본다.
    _, predicted = torch.max(outputs.data, 1)
    print('Predicted:', ' '.join('%5s' % classes[predicted[j]] for j in range(4)))
    # (5)

    # performance on the whole test dataset
    # dataSet에 대해서 어떻게 동작하는지 본다.
    correct = 0
    total = 0
    for data in testloader:
        images, labels = data
        outputs = net(Variable(images))
        _, predicted = torch.max(outputs.data, 1)
        total += labels.size(0)
        correct += (predicted == labels).sum()

    print('Accuracy of the network on the 10000 test images: %d %%'
          % (100 * correct / total))
    # (6)

    # performance on each class
    # 어떤 것들을 더 잘 분류하고, 어떤 것들을 더 못했는지 알아본다.
    class_correct = list(0. for i in range(10))
    class_total = list(0. for i in range(10))
    for data in testloader:
        images, labels = data
        outputs = net(Variable(images))
        _, predicted = torch.max(outputs.data, 1)
        c = (predicted == labels).squeeze()
        for i in range(4):
            label = labels[i]
            class_correct[label] += c[i]
            class_total[label] += 1

    for i in range(10):
        print('Accuracy of %5s : %2d %%' % (
            classes[i], 100 * class_correct[i] / class_total[i]))
    # (7)
