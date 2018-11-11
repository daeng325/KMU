###############################################오류 역전파2 ################################
import torch

x = torch.randn(1,10)
prev_h = torch.randn(1,20)
w_h = torch.randn(20,20,requires_grad=True)
w_x = torch.randn(20,10,requires_grad=True)

i2h = torch.mm(w_x, x.t())  #If mat1 is a (n×m) tensor, mat2 is a (m×p) tensor, out will be a (n×p) tensor.
h2h = torch.mm(w_h, prev_h.t()) #x.t(), prev_h.t() : 전치행렬을 구함.
next_h = i2h + h2h
next_h = next_h.tanh()

loss=next_h.sum()
loss.backward()
print(loss)
# print(w_h.grad)
# print(w_x.grad)

