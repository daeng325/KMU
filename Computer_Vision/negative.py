# -*- coding: utf-8 -*-
import cv2
import numpy as np

FILENAME = 'coin1.bmp'
image = cv2.imread(FILENAME, cv2.IMREAD_COLOR)
# output_image = 255 - image # negative
output_image = 30 * np.uint8(np.log1p(abs(image)))# uint8	Unsigned integer (0 to 255)
cv2.imshow('image', image)
cv2.imshow('output', output_image)
cv2.waitKey(0)
cv2.destroyAllWindows()

#
# FILENAME = 'coin1.bmp'
# # 이미지 파일을 읽기 위한 객체를 리턴  인자(이미지 파일 경로, 읽기 방식)
# # cv2.IMREAD_COLOR : 투명한 부분 무시되는 컬러
# # cv2.IMREAD_GRAYSCALE : 흑백 이미지로 로드
# # cv2.IMREAD_UNCHANGED : 알파 채컬을 포함한 이미지 그대로 로드
# image = cv2.imread(FILENAME, cv2.IMREAD_COLOR)
# output = cv2.bitwise_not(image)
# output_image = 255 - image
# # cv2.namedWindow('model', cv2.WINDOW_NORMAL)  # 윈도우 창의 성격 지정 인자 : (윈도우타이틀, 윈도우 사이즈 플래그) , 사용자가 크기 조절할 수 있는 윈도우 생성
# cv2.imshow('image', image)  # 화면에 이미지 띄우기 인자;(윈도우타이틀, 이미지객체)
# cv2.imshow('output',output_image)
# cv2.waitKey(0)  # 지정된 시간 동안 키보드 입력 대기 (ms) , 0이면 key 입력할 때 까지 계속 대기, 입력받은 key 값을 int 로 반환 (아스키코드)
# # 대기시간이 끝나면 아래 코드로 진행
# cv2.destroyAllWindows()  # 생성한 윈도우 제거

