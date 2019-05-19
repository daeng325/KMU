########################데이터 전처리 ###############################
import torch
from torch.utils.data import TensorDataset, DataLoader
#dataloader는 데이터 집합의 minibatching, shuffling, multithreading등을 제공해준다.

class TwoLayerNet(torch.nn.Module):
    def __init__(self,D_in, H, D_out):
        super(TwoLayerNet,self).__init__()
        self.linear1 = torch.nn.Linear(D_in, H)
        self.linear2 = torch.nn.Linear(H, D_out)

    def forward(self,x):
        h_relu = self.linear1(x).clamp(min=0)
        y_pred = self.linear2(h_relu)
        return y_pred

N, D_in, H, D_out = 64, 1000, 100, 10

x = torch.randn(N, D_in)
y = torch.randn(N, D_out)


'''
dataloaders = {x: torch.utils.data.DataLoader(image_datasets[x], batch_size=4,
                                             shuffle=True, num_workers=4)
              for x in ['train', 'val']}
dataset_sizes = {x: len(image_datasets[x]) for x in ['train', 'val']}
class_names = image_datasets['train'].classes
[출처] 파이토치 튜토리얼(11)/ Transfer Learning tutorial / BEGINNER TUTORIALS|작성자 발꿈치 대마왕


'''

loader = DataLoader(TensorDataset(x,y), batch_size = 8) #define dataloader
model = TwoLayerNet(D_in, H, D_out)

optimizer = torch.optim.SGD(model.parameters(), lr=1e-2)

for epoch in range(20):
    for x_batch, y_batch in loader:
        y_pred = model(x_batch)
        loss = torch.nn.functional.mse_loss(y_pred,y_batch)

        loss.backward()
        optimizer.step()
        optimizer.zero_grad()