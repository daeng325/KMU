#-*- coding:utf-8 -*-
import cv2
import numpy as np
from matplotlib import pyplot as plt

FILENAME = 'coin1-blur.bmp'
img = cv2.imread(FILENAME , cv2.IMREAD_COLOR)

laplacian = cv2.Laplacian(img,cv2.CV_8U)
# laplacian_kernel = np.array([[-1,-1,-1],
#                             [-1,9,-1],
#                             [-1,-1,-1]])

# laplacian_img = cv2.filter2D(img,-1,laplacian_kernel)
cv2.imshow('img',img)
# cv2.imshow('laplacian img',laplacian_img)
cv2.imshow('laplacian img',laplacian)
cv2.waitKey(0)
cv2.destroyAllWindows()