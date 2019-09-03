# FT with numpy

import cv2
import numpy as np
import matplotlib.pyplot as plt

FILENAME = 'coin1.bmp'
img = cv2.imread(FILENAME,0)
f = np.fft.fft2(img) # 이미지에 푸리에 변환 적용
fshift = np.fft.fftshift(f) #분석을 용이하게 하기 위해 주파수가 0인 부분을 중앙에 위치시킴. 중앙에 저주파가 모이게 됨.
eps = 1e-08
magnitude_spectrum = 20*np.log(np.abs(fshift+eps))

plt.subplot(121),plt.imshow(img, cmap = 'gray')
plt.title('Input Image'), plt.xticks([]), plt.yticks([])
plt.subplot(122),plt.imshow(magnitude_spectrum, cmap = 'gray')
plt.title('Magnitude Spectrum'), plt.xticks([]), plt.yticks([])
plt.show()



### FT with openCV
# import numpy as np
# import cv2
# from matplotlib import pyplot as plt
#
# FILENAME = 'coin1.bmp'
# img = cv2.imread(FILENAME,0)
#
# dft = cv2.dft(np.float32(img),flags = cv2.DFT_COMPLEX_OUTPUT)
# #중심이동
# dft_shift = np.fft.fftshift(dft)
#
# magnitude_spectrum = 20*np.log(cv2.magnitude(dft_shift[:,:,0],dft_shift[:,:,1]))
#
# plt.subplot(121),plt.imshow(img, cmap = 'gray')
# plt.title('Input Image'), plt.xticks([]), plt.yticks([])
# plt.subplot(122),plt.imshow(dft, cmap = 'gray')
# plt.title('Magnitude Spectrum'), plt.xticks([]), plt.yticks([])
# plt.show()