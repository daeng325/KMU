import numpy as np
import cv2
from matplotlib import pyplot as plt

FILENAME = 'coin1.bmp'
img = cv2.imread(FILENAME,0)

dft = cv2.dft(np.float32(img),flags = cv2.DFT_COMPLEX_OUTPUT)
dft_shift = np.fft.fftshift(dft)


rows, cols = img.shape
crow, ccol = rows//2 , cols//2     # center

# create a mask first, center square is 1, remaining all zeros
d = 30
mask = np.zeros((rows, cols, 2), np.uint8)
mask[crow-d:crow+d, ccol-d:ccol+d] = 1
high_pass_mask = 1-mask
HPEF_mask = 0.5 + (high_pass_mask*0.75)




# apply mask and inverse DFT
fshift = dft_shift*HPEF_mask
f_ishift = np.fft.ifftshift(fshift)

inverse_FFT_img= 20*np.log(cv2.magnitude(f_ishift[:,:,0],f_ishift[:,:,1]))
cv2.imshow('inverseFFT',inverse_FFT_img)
cv2.waitKey(0)
cv2.destroyAllWindows()


img_back = cv2.idft(f_ishift)
img_back = cv2.magnitude(img_back[:,:,0],img_back[:,:,1])

plt.subplot(121),plt.imshow(img, cmap = 'gray')
plt.title('Input Image'), plt.xticks([]), plt.yticks([])
plt.subplot(122),plt.imshow(img_back, cmap = 'gray')
plt.title('Inverse HPEF'), plt.xticks([]), plt.yticks([])
plt.show()