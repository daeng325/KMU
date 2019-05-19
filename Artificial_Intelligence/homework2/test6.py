###########################################신경망 학습3 #################################
import torch

class MyReLU(torch.autograd.Function):
    '''
    torch.autograd.Function을 상속받아 사용자 정의 자동미분 함수를 구현한다.
    '''
    @staticmethod
    def forward(ctx, x):
        ctx.save_for_backward(x) # 역전파 단계에서 사용할 오브젝트 저장
        return x.clamp(min=0) # relu 적용한 값을 리턴.

    @staticmethod
    def backward(ctx,grad_y):
        x, = ctx.saved_tensors
        grad_input = grad_y.clone()
        grad_input[x<0] = 0
        return grad_input


def my_relu(x):
    return MyReLU.apply(x) # Function.apply 메소드를 사용해 사용자 정의 함수 구현.

N, D_in, H, D_out = 64, 1000, 100, 10

x = torch.randn(N,D_in)
y = torch.randn(N, D_out)
w1 = torch.randn(D_in, H, requires_grad = True)
w2 = torch.randn(H, D_out, requires_grad = True)

learning_rate = 1e-6
for t in range(500):
    y_pred = my_relu(x.mm(w1)).mm(w2)
    loss = (y_pred-y).pow(2).sum()

    # autograde를 사용해 역전파 단계 계산
    loss.backward()

    with torch.no_grad():
        w1 -= learning_rate * w1.grad
        w2 -= learning_rate * w2.grad
        w1.grad.zero_()
        w2.grad.zero_()
