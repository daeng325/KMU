# #-*- coding:utf-8 -*-
import cv2
import numpy as np
import matplotlib.pyplot as plt


FILENAME = 'coin1-spnoise3.bmp'
img = cv2.imread(FILENAME , cv2.IMREAD_COLOR)

# pyplot를 사용하기 위해서 BGR을 RGB로 변환.
b,g,r = cv2.split(img)
img = cv2.merge([r,g,b])

# 일반 Blur
blur_img = cv2.blur(img,(7,7))

# GaussianBlur
gaussian_blur_img = cv2.GaussianBlur(img,(5,5),0) #cv2.GaussianBlur(img, ksize, sigmaX)
gaussian2_blur_img = cv2.GaussianBlur(gaussian_blur_img,(5,5),0)

# Median Blur
median_blur_img = cv2.medianBlur(img,9)

# Bilateral Filtering
bilateral_img = cv2.bilateralFilter(img,9,75,75)


images = [img,blur_img,gaussian_blur_img,gaussian2_blur_img,median_blur_img,bilateral_img]
titles=['Original','Blur(7X7)','Gaussian Filter(5X5)','Gaussian Filter (5X5) X2','Median Filter','Bilateral Filter']

for i in range(6):
    plt.subplot(3,2,i+1),plt.imshow(images[i]),plt.title(titles[i])
    plt.xticks([]),plt.yticks([])

plt.show()


# cv2.imshow('img',img)
# cv2.imshow('blur',blur_img)
# cv2.imshow('gaussian',gaussian_blur_img)
# cv2.imshow('gaussian2',gaussian2_blur_img)
# cv2.imshow('median',median_blur_img)
# cv2.imshow('bilateral',bilateral_img)
# cv2.waitKey(0)
# cv2.destroyAllWindows()



#########################
# import cv2
# import numpy as np
#
# def nothing(x):
#     pass
#
# FILENAME = 'coin1-blur.bmp'
# img = cv2.imread(FILENAME , cv2.IMREAD_COLOR)
#
# cv2.namedWindow('image')
# cv2.createTrackbar('K','image',1,20, nothing)
#
# while(1):
#     if cv2.waitKey(1) & 0xFF == 27:
#         break
#     k = cv2.getTrackbarPos('K','image')
#
#     #(0,0)이면 에러가 발생함으로 1로 치환
#     if k == 0:
#         k = 1
#
#     # trackbar에 의해서 (1,1) ~ (20,20) kernel생성
#     kernel = np.ones((k,k),np.float32)/(k*2)
#     dst = cv2.filter2D(img,-1,kernel)
#
#     cv2.imshow('image',dst)
#
# cv2.destroyAllWindows()
