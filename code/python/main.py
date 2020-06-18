#coding:utf-8
import cv2
import numpy as np
import glob
 
# 找棋盤格角點
# 閾值
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
#棋盤格模板規格
w = 9
h = 6
# 世界座標系中的棋盤格點,例如(0,0,0), (1,0,0), (2,0,0) ....,(8,5,0)，去掉Z座標，記為二維矩陣
objp = np.zeros((w*h,3), np.float32)
objp[:,:2] = np.mgrid[0:w,0:h].T.reshape(-1,2)
# 儲存棋盤格角點的世界座標和影象座標對
objpoints = [] # 在世界座標系中的三維點
imgpoints = [] # 在影象平面的二維點
 
images = glob.glob('calib/*.png')
for fname in images:
 img = cv2.imread(fname)
 gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
 # 找到棋盤格角點
 ret, corners = cv2.findChessboardCorners(gray, (w,h),None)
 # 如果找到足夠點對，將其儲存起來
 if ret == True:
  cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
  objpoints.append(objp)
  imgpoints.append(corners)
  # 將角點在影象上顯示
  cv2.drawChessboardCorners(img, (w,h), corners, ret)
  cv2.imshow('findCorners',img)
  cv2.waitKey(1)
cv2.destroyAllWindows()
 
# 標定
ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1], None, None)
 
# 去畸變
img2 = cv2.imread('calib/00169.png')
h, w = img2.shape[:2]
newcameramtx, roi=cv2.getOptimalNewCameraMatrix(mtx,dist,(w,h),0,(w,h)) # 自由比例引數
dst = cv2.undistort(img2, mtx, dist, None, newcameramtx)
# 根據前面ROI區域裁剪圖片
#x,y,w,h = roi
#dst = dst[y:y+h, x:x+w]
cv2.imwrite('calibresult.png',dst)
 
# 反投影誤差
total_error = 0
for i in xrange(len(objpoints)):
 imgpoints2, _ = cv2.projectPoints(objpoints[i], rvecs[i], tvecs[i], mtx, dist)
 error = cv2.norm(imgpoints[i],imgpoints2, cv2.NORM_L2)/len(imgpoints2)
 total_error += error
print "total error: ", total_error/len(objpoints)