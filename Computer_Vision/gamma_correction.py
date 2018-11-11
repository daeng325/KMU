# #-*- coding:utf-8 -*-
import cv2

img = cv2.imread('coin1.bmp')
img = img / 255.0
gamma = 1.5
im_power_law_transformation = cv2.pow(img, gamma)
cv2.imshow('Original Image', img)
cv2.imshow('Power Law Transformation', im_power_law_transformation)
cv2.waitKey(0)
cv2.destroyAllWindows()