##다차항 회귀 ##
import numpy.random as md
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
from sklearn.linear_model import LinearRegression

##다차항 회귀 ##

np.random.seed(42)
m = 100
X = 6 * np.random.rand(m,1)-3
y= 0.5 * X**2 + X + 2 + np.random.randn(m,1) #2차식
# plt.plot(X,y,"b.")
# plt.xlabel("$x_1$",fontsize=18)
# plt.ylabel("$y$",rotation=0, fontsize=18)
# plt.axis([-3,3,0,10])
# plt.show()

from sklearn.preprocessing import PolynomialFeatures ##입력값 x를 다항식으로 변환한다.
poly_features = PolynomialFeatures(degree=2, include_bias=False) #상수항이 없는 2차 다항식으로 변환
X_poly = poly_features.fit_transform(X)
print("X[0] is \n",X[0])
#
print("X_poly[0] is \n", X_poly[0])
# 예를들면 x_poly[0][0]은 x[0]이고 x_poly[0][1]은 x[0]**2인 거!

lin_reg = LinearRegression()
lin_reg.fit(X_poly,y)
print("int_reg.intercept, lin_reg.coef is \n", lin_reg.intercept_, lin_reg.coef_)
#선형회귀를 통해 절편과 계수를 구했음. 계수는 2차이므로 2개(1차계수, 2차계수)가 나옴.


X_new = np.linspace(-3,3,100).reshape(100,1) #linspace : 선형구간을 지정한 구간의 수만큼 분할 (시작,끝(포함),갯수)
X_new_poly = poly_features.transform(X_new)
y_new = lin_reg.predict(X_new_poly) #새로운 입력 데이터에 대한 출력 데이터 예측
# plt.plot(X,y,"b.")
# plt.plot(X_new, y_new, "r-", linewidth=2, label="prediction")
# plt.xlabel("$x_1$",fontsize=18)
# plt.ylabel("$y$",rotation=0, fontsize=18)
# plt.legend(loc="upper left", fontsize=14)
# plt.axis([-3,3,0,10])
# plt.show()
# #실제값과 예측값 비교


from sklearn.preprocessing import StandardScaler
from sklearn.pipeline import Pipeline
#파이프라인 기능을 이용해 분류 모형과 합칠 수 있다.
#표준 스케일러와 로지스틱 회귀 모형
#스케일링 : 자료 집합에 적용되는 전처리 과정으로 모든 자료에 선형 변환을 적용하여 전체 자료의 분포를 평균 0, 분산 1이 되도록 만드는 과정이다.

for style, width, degree in (("g-",1,300),("b--",2,2),("r-+",2,1)):
    polybig_features = PolynomialFeatures(degree=degree, include_bias=False)
    std_scaler = StandardScaler()
    lin_reg = LinearRegression()
    polynomial_regression = Pipeline([
        ("poly_features",polybig_features),
        ("std_scaler", std_scaler),
        ("lin_reg",lin_reg),
    ])
    polynomial_regression.fit(X,y)
    y_newbig = polynomial_regression.predict(X_new)
    plt.plot(X_new,y_newbig,style, label=str(degree), linewidth=width)
# plt.plot(X,y,"b.",linewidth =3)
# plt.legend(loc="upper left")
# plt.xlabel("$x_1$",fontsize=18)
# plt.ylabel("$y$",rotation=0, fontsize=18)
# plt.axis([-3,3,0,10])
# plt.show()

#300차 , 2차, 1차 다항식과 X,y 실제값을 그래프에 함께 표현했다.