#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<opencv2\calib3d\calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include<string>
#include<vector>
class CameraCalibrator
{
private:
  //世界座標
  std::vector < std::vector<cv::Point3f >> objectPoints;
  //影象座標
  std::vector <std::vector<cv::Point2f>> imagePoints;
  //輸出矩陣
  cv::Mat camerMatirx;
  cv::Mat disCoeffs;
  //標記
  int flag;
  //去畸變引數
  cv::Mat map1, map2;
  //是否去畸變
  bool mustInitUndistort;
 
  ///儲存點資料
  void addPoints(const std::vector<cv::Point2f>&imageConers, const std::vector<cv::Point3f>&objectConers)
  {
    imagePoints.push_back(imageConers);
    objectPoints.push_back(objectConers);
  }
public:
  CameraCalibrator() :flag(0), mustInitUndistort(true){}
  //開啟棋盤圖片，提取角點
  int addChessboardPoints(const std::vector<std::string>&filelist,cv::Size &boardSize)
  {
    std::vector<cv::Point2f>imageConers;
    std::vector<cv::Point3f>objectConers;
    //輸入角點的世界座標
    for (int i = 0; i < boardSize.height; i++)
    {
      for (int j = 0; j < boardSize.width; j++)
      {
        objectConers.push_back(cv::Point3f(i, j, 0.0f));
      }
    }
    //計算角點在影象中的座標
    cv::Mat image;
    int success = 0;
    for (int i = 0; i < filelist.size(); i++)
    {
      image = cv::imread(filelist[i],0);
      //找到角點座標
      bool found = cv::findChessboardCorners(image, boardSize, imageConers);
      cv::cornerSubPix(image,
        imageConers,
        cv::Size(5, 5),
        cv::Size(-1, -1),
        cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS,
        30, 0.1));
      if (imageConers.size() == boardSize.area())
      {
        addPoints(imageConers, objectConers);
        success++;
      }
      //畫出角點
      cv::drawChessboardCorners(image, boardSize, imageConers, found);
      cv::imshow("Corners on Chessboard", image);
      cv::waitKey(100);
    }
    return success;
  }
 
  //相機標定
  double calibrate(cv::Size&imageSize)
  {
    mustInitUndistort = true;
    std::vector<cv::Mat>rvecs, tvecs;
    //相機標定
    return cv::calibrateCamera(objectPoints, imagePoints, imageSize,
      camerMatirx, disCoeffs, rvecs, tvecs, flag);
  }
  ///去畸變
  cv::Mat remap(const cv::Mat &image)
  {
    cv::Mat undistorted;
    if (mustInitUndistort)
    {
      //計算畸變引數
      cv::initUndistortRectifyMap(camerMatirx, disCoeffs,
        cv::Mat(), cv::Mat(), image.size(), CV_32FC1, map1, map2);
      mustInitUndistort = false;
    }
    //應用對映函式
    cv::remap(image, undistorted, map1, map2, cv::INTER_LINEAR);
    return undistorted;
  }
  //常成員函式，獲得相機內參數矩陣、投影矩陣資料
  cv::Mat getCameraMatrix() const { return camerMatirx; }
  cv::Mat getDistCoeffs()  const { return disCoeffs; }
};