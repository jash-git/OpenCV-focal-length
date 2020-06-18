#include"CameraCalibrator.h"
#include<iomanip>
#include<iostream>
int main()
{
  CameraCalibrator Cc;
  cv::Mat image;
  std::vector<std::string> filelist;
  cv::namedWindow("Image");
  for (int i = 1; i <= 22; i++)
  {
    ///讀取圖片
    std::stringstream s;
    s << "D:/images/chessboards/chessboard" << std::setw(2) << std::setfill('0') << i << ".jpg";
    std::cout << s.str() << std::endl;
 
    filelist.push_back(s.str());
    image = cv::imread(s.str(),0);
    cv::imshow("Image", image);
    cv::waitKey(100);
  }
  //相機標定
  cv::Size boardSize(6, 4);
  Cc.addChessboardPoints(filelist, boardSize);
  Cc.calibrate(image.size());
 
  //去畸變
  image = cv::imread(filelist[1]);
  cv::Mat uImage=Cc.remap(image);
  cv::imshow("原影象", image);
  cv::imshow("去畸變", uImage);
  //顯示相機內參數矩陣
  cv::Mat cameraMatrix = Cc.getCameraMatrix();
  std::cout << " Camera intrinsic: " << cameraMatrix.rows << "x" << cameraMatrix.cols << std::endl;
  std::cout << cameraMatrix.at<double>(0, 0) << " " << cameraMatrix.at<double>(0, 1) << " " << cameraMatrix.at<double>(0, 2) << std::endl;
  std::cout << cameraMatrix.at<double>(1, 0) << " " << cameraMatrix.at<double>(1, 1) << " " << cameraMatrix.at<double>(1, 2) << std::endl;
  std::cout << cameraMatrix.at<double>(2, 0) << " " << cameraMatrix.at<double>(2, 1) << " " << cameraMatrix.at<double>(2, 2) << std::endl;
 
  cv::waitKey(0);
}