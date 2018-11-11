import cv2
import numpy as np

FILENAME = 'coin1.bmp'
image = cv2.imread(FILENAME, cv2.IMREAD_COLOR)
output_image = np.uint8(np.exp(abs(image)/30) -1)
cv2.imshow('image', image)
cv2.imshow('output', output_image)
cv2.waitKey(0)
cv2.destroyAllWindows()
