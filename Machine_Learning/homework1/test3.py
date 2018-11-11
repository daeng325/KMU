import numpy.random as md
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
from sklearn.linear_model import LinearRegression

from sklearn.preprocessing import PolynomialFeatures ##입력값 x를 다항식으로 변환한다.

from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import Pipeline

## 규제 ##
from sklearn.linear_model import Ridge

#선형회귀 계수에 대한 제약 조건을 추가함으로써 모형이 오버피팅 되는 것을 막는다.

#Ridge 가중치들의 제곱합을 최소화해야 하는 것을 제약조건으로 한다.

np.random.seed(42)
m = 20
X = 3 * np.random.rand(m,1)
y = 1 + 0.5 * X + np.random.randn(m,1) / 1.5
X_new = np.linspace(0,3,100).reshape(100,1) #0부터 3사이의 임의의 수 100개
def plot_model(model_class, polynomial, alphas, **model_kargs): #**kwargs : 딕셔너리 형태로 입력?
    for alpha, style in zip(alphas, ("b-","g--","r:")):
        model = model_class(alpha, **model_kargs) if alpha > 0 else LinearRegression()
        if polynomial:
            model = Pipeline([
                ("poly_features",PolynomialFeatures(degree=10, include_bias=False)),
                ("std_scaler",StandardScaler()),
                ("regul_reg",model),
            ])
        model.fit(X,y)
        y_new_regul = model.predict(X_new)
        lw = 2 if alpha > 0 else 1
        plt.plot(X_new, y_new_regul, style, linewidth=lw, label=r"$\alpha = {}$".format(alpha))
    plt.plot(X,y,"b.",linewidth=3)
    plt.legend(loc="upper left", fontsize=15)
    plt.xlabel("$x_1$",fontsize=18)
    plt.axis([0,3,0,4])
plt.figure(figsize=(8,4))
plt.subplot(121)
plot_model(Ridge,polynomial=False, alphas=(0,10,100),random_state=42)
plt.ylabel("$y$",rotation=0, fontsize=18)
plt.subplot(122)
plot_model(Ridge, polynomial=True, alphas=(0,10**-5,1),random_state=42)
plt.show()

# alpha 가 커지면 커질수록 penalty또한 커지게 되면서 계수의 크기가 줄어들게 된다.