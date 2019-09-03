# #-*-coding:utf-8-*-
# import cv2
# import numpy as np
# import matplotlib.pyplot as plt
#
# FILENAME = 'coin1.bmp'
# img = cv2.imread(FILENAME, cv2.IMREAD_COLOR)
#
# hist, bins = np.histogram(img.flatten(), 256,[0,256])
#
# cdf = hist.cumsum()
#
# # cdf의 값이 0인 경우는 mask처리를 하여 계산에서 제외
# # mask처리가 되면 Numpy 계산에서 제외가 됨
# # 아래는 cdf array에서 값이 0인 부분을 mask처리함
# cdf_m = np.ma.masked_equal(cdf,0)
#
# #History Equalization 공식
# cdf_m = (cdf_m - cdf_m.min())*255/(cdf_m.max()-cdf_m.min())
#
# # Mask처리를 했던 부분을 다시 0으로 변환
# cdf = np.ma.filled(cdf_m,0).astype('uint8')
#
# img2 = cdf[img]
# cv2.imshow('img',img)
# cv2.imshow('histogram_equalized image',img2)
# cv2.waitKey(0)
# cv2.destroyAllWindows()
#
# plt.subplot(121),plt.imshow(img),plt.title('Original')
# plt.subplot(122),plt.imshow(img2),plt.title('Equalization')
# plt.show()

###############################

#-*-coding:utf-8-*-
import cv2
import numpy as np
import matplotlib.pyplot as plt

FILENAME = 'coin1.bmp'
img = cv2.imread(FILENAME,0);

# OpenCV의 Equaliztion함수
img2 = cv2.equalizeHist(img)

plt.hist(img.ravel(), 256, [0,256])
plt.show()
plt.hist(img2.ravel(), 256, [0,256])
plt.show()

img = cv2.resize(img,(400,400))
img2 = cv2.resize(img2,(400,400))

dst = np.hstack((img, img2))
cv2.imshow('img',dst)
cv2.waitKey(0)
cv2.destroyAllWindows()

