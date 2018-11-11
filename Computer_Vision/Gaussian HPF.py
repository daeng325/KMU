import numpy as np
import cv2

img = cv2.imread('coin1.bmp')
npFFT = np.fft.fft2(img) # Calculate FFT
npFFTS = np.fft.fftshift(npFFT)  # Shift the FFT to center it

#High-pass Gaussian filter
(P, Q) = npFFTS.shape
H = np.zeros((P,Q))
D0 = 30
for u in range(P):
    for v in range(Q):
        H[u, v] = 1.0 - np.exp(- ((u - P / 2.0) ** 2 + (v - Q / 2.0) ** 2) / (2 * (D0 ** 2)))
k1 = 0.5 ; k2 = 0.75
HFEfilt = k1 + k2 * H # Apply High-frequency emphasis

# Apply HFE filter to FFT of original image
HFE = HFEfilt * npFFTS

#Perform IFFT (implemented here using the np.fft function)
HFEfinal = (np.conjugate(np.fft2d(np.conjugate(HFE)))) / (P * Q)