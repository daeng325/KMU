import cv2
import numpy as np

FILENAME = 'coin1.bmp'
image = cv2.imread(FILENAME, cv2.IMREAD_COLOR)
output_image = 30 *  np.uint8(np.log1p(abs(image)))# uint8	Unsigned integer (0 to 255)
cv2.imshow('image', image)
cv2.imshow('output', output_image)
cv2.waitKey(0)
cv2.destroyAllWindows()
