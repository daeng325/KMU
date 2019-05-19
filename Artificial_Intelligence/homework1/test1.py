import numpy as np
#%matplotlib inline
import matplotlib
import matplotlib.pyplot as plt


X = 2 * np.random.rand(100,1) #0부터 1 사이의 균일분포 난수.
y = 4 + 3*X + np.random.randn(100,1) #가우시안 표준 정규 분포
# plt.plot(X,y,"b.")
# plt.xlabel("$x_1$",fontsize=18)
# plt.ylabel("$y$",rotation=0,fontsize=18)
# plt.axis([0,2,0,15])
# plt.show()

X_b = np.c_[np.ones((100,1)),X] ##배열을 column기준으로 붙였다.
theta_best = np.linalg.inv(X_b.T.dot(X_b)).dot(X_b.T).dot(y)
##inv 역행렬 // X_b의 역행렬 과 y를 곱했다.
print("theta_best is \n" ,theta_best)

X_new = np.array([[0],[2]])
X_new_b = np.c_[np.ones((2,1)),X_new]
y_predict = X_new_b.dot(theta_best)
print("y_predict is \n", y_predict)
## np.array([1,0],[1,2]) 와 theta_best를 곱했다. => y_predict

# plt.plot(X_new,y_predict,"r-",linewidth=2, label="prediction")
# plt.plot(X,y,"b.")
# plt.xlabel("$x_1$",fontsize=18)
# plt.ylabel("$y",rotation = 0, fontsize=18)
# plt.legend(loc="upper left",fontsize=14)
# plt.axis([0,2,0,15])
# plt.show()

from sklearn.linear_model import LinearRegression


lin_reg = LinearRegression()
lin_reg.fit(X,y) #모델을 추정해서 객체를 반환한다.
print("lin_reg.intercept_, lin_reg.coef_ is \n",lin_reg.intercept_, lin_reg.coef_)
##coef_ 추정된 가중치벡터 ,  intercept_ 추정되는 상수항 b

print("lin_reg.predict(X_new) is \n",lin_reg.predict(X_new))
#새로운 입력 데이터에 대한 출력 데이터 예측



## 실수 행렬의 특이값 분해 계산
## 특이값 분해는 어떤 행렬을 세 행렬의 곱으로 분해하는 것이다.
theta_best_svd, residuals, rank, s = np.linalg.lstsq(X_b, y, rcond=1e-6)
## y=xb를 만족하는 최소제곱해
## p=최소자승해, residuals=잔차(경험적 관찰로 얻은 값과 모델로 추정된 값의 차, r=랭크 , s=특이값

print("theta_best_svd is \n",theta_best_svd)
#
print("np.linalg.pinv(X_b).dot(y) is \n",np.linalg.pinv(X_b).dot(y))
## np.linalg.pinv 행렬의 무어=펜로즈 유사역원 역행렬을 계산한다...???
## 정사각행렬이 아닌 행렬의 역행렬을 구한다는 건가? 그거랑 y랑 왜 곱해?
##X_b는 1쭈르륵이랑 X랑 붙인 거!
##어 근데 이거 theta_best_svd 랑 값이 똑같은데?!?!
## => X_b *x = y 인 x를 찾기 위한 과정이었다!!! theta_best_svd도 마찬가지고.

### 경사 하강법을 사용한 선형회귀 접근 ###
eta = 0.1 #학습률
n_iterations = 1000
m = 100
theta = np.random.randn(2,1)
for iteration in range(n_iterations):
    gradients = 2/m * X_b.T.dot(X_b.dot(theta)-y) # 아 이거 그거네 손실함수를 편미분한거!!!
    theta = theta - eta * gradients
print("gradient desent result theta is \n", theta)
## 함수의 기울기를 구하여 기울기가 낮은 쪽으로 반복적으로 이동하여 극값에 도달

print("X_new_b.dot(theta) is \n", X_new_b.dot(theta))
##위에서는 y_predict 가 X_new_b랑 theta_best랑 곱한 거였는데...
##와 진짜  y_predict랑 같아....와우


theta_path_bgd = [] ##배치 경사 하강 알고리즘
def plot_gradient_descent(theta, eta, theta_path=None):
    m = len(X_b) #m은 다 100임.
    plt.plot(X,y,"b.")
    n_iterations = 1000
    for iteration in range(n_iterations):
        if iteration <10:
            y_predict = X_new_b.dot(theta)
            style = "b-" if iteration >0 else "r--"
            plt.plot(X_new, y_predict, style)
        gradients = 2/m * X_b.T.dot(X_b.dot(theta)-y)
        theta = theta - eta * gradients
        if theta_path is not None:
            theta_path.append(theta)
    plt.xlabel("$x_1$",fontsize=18)
    plt.axis([0,2,0,15])
    plt.title(r"$\eta = {}$".format(eta),fontsize=16)
np.random.seed(42) #시작 숫자 42 생성된 난수는 다음번 난수 생성을 위한 시드값.
theta=np.random.randn(2,1)
# plt.figure(figsize=(10,4))
# plt.subplot(131); plot_gradient_descent(theta,eta=0.02) #eta == 학습률
# plt.ylabel("$y$",rotation=0, fontsize=18)
# plt.subplot(132);
plot_gradient_descent(theta,eta=0.1, theta_path = theta_path_bgd)
# plt.subplot(133); plot_gradient_descent(theta, eta=0.5)
# # plt.show()

##극값의 세타를 넣어줬다??ㅇㅅㅇ??
## 예측값과 가장 잘 맞는 학습률일 때 세타 bgd[] 에 넣어줬다.



# 스토캐스틱 경사 하강법을 사용한 선형회귀 접근 #
theta_path_sgd = []
m=len(X_b)
np.random.seed(42)
n_epochs = 50
t0,t1 = 5,50
def learning_schedule(t):
    return t0 /(t+t1)
theta = np.random.randn(2,1)
for epoch in range(n_epochs): #epoch:세대
    for i in range(m):
        if epoch == 0 and i<20: # 진행할수록 기울기가...점점 목표값에 가까워지니까 많이 볼 필요 없어서 20까지만 보는 건가?
            y_predict = X_new_b.dot(theta)
            style = "b-" if i>0 else "r--"
            #plt.plot(X_new, y_predict, style)
        random_index = np.random.randint(m)
        xi = X_b[random_index:random_index+1] # x_b[random_index]를 뽑은 것이다..
        yi = y[random_index:random_index+1]
        gradients = 2*xi.T.dot(xi.dot(theta)-yi) #랜덤수를 정해서 몇번째 샘플에 대한 그레디언트를 계산
        eta = learning_schedule(epoch * m +i) #학습률 왜 계산? ㅇㅅㅇ?
        theta = theta - eta * gradients
        theta_path_sgd.append(theta) #50개를 다 ...넣었다!
# plt.plot(X,y,"b.")
# plt.xlabel("$x_1$", fontsize=18)
# plt.ylabel("$y$", rotation =0, fontsize=18)
# plt.axis([0,2,0,15])
# plt.show()

print("stochastic gradient descent result is \n",theta) #스토캐스틱 경사 하강 알고리즘 최적해

from sklearn.linear_model import SGDRegressor #스토캐스틱 라이브러리?
sgd_reg = SGDRegressor(max_iter=50, penalty=None, eta0=0.1, random_state=42)
print("sgd_reg.fit(X,y,ravel()) is \n",sgd_reg.fit(X,y.ravel()))
#
print("sgd_reg.intercept_, sgd_reg.coef_ is \n",sgd_reg.intercept_,sgd_reg.coef_)
## wx+b intercept:절편 / coefficient : 계수


### 미니배치 경사 하강법을 사용한 선형회귀 접근 ###
theta_path_mgd=[]
n_itetations = 50
minibatch_size = 20
np.random.seed(42)
theta = np.random.randn(2,1)
t0,t1 = 200, 1000
def learning_schedule(t):
    return t0/(t+ t1)
t=0
for epoch in range (n_iterations):
    shuffled_indices = np.random.permutation(m) #주어진 인자 미만의 정수(0포함) 로 순열을 만드는 함수.
    X_b_shuffled = X_b[shuffled_indices] #xb의 index를 섞은 게 shuffled_indices/
    y_shuffled = y[shuffled_indices]
    for i in range(0,m,minibatch_size):
        t += 1
        xi = X_b_shuffled[i:i+minibatch_size]
        yi = y_shuffled[i:i+minibatch_size]
        gradients = 2/minibatch_size * xi.T.dot(xi.dot(theta)-yi)
        eta = learning_schedule(t)
        theta = theta - eta * gradients
        theta_path_mgd.append(theta)
print("mini-batch gradient descent result is \n", theta)

theta_path_bgd = np.array(theta_path_bgd)
theta_path_sgd = np.array(theta_path_sgd)
theta_path_mgd = np.array(theta_path_mgd)

plt.figure(figsize=(7,4))
plt.plot(theta_path_sgd[:,0],theta_path_sgd[:,1],"r-s",linewidth=1, label="SGD")
plt.plot(theta_path_mgd[:,0], theta_path_mgd[:,1],"g-+", linewidth=2, label="MINI_BATCH")
plt.plot(theta_path_bgd[:,0], theta_path_bgd[:,1],"b-o",linewidth=3, label="BATCH")
plt.legend(loc="upper left",fontsize=16)
plt.xlabel(r"$\theta_0$",fontsize=20)
plt.ylabel(r"$\theta_1$     ",fontsize=20, rotation=0)
plt.axis([2.5, 4.5, 2.3, 3.9])
plt.show()
