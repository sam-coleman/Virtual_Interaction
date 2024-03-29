#include <iostream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


int minH = 0, maxH = 20, minS = 30, maxS = 150, minV = 60, maxV = 255;
cv::Mat frame;
int count = 0;

float innerAngle(float px1, float py1, float px2, float py2, float cx1, float cy1)
{

 float dist1 = std::sqrt(  (px1-cx1)*(px1-cx1) + (py1-cy1)*(py1-cy1) );
 float dist2 = std::sqrt(  (px2-cx1)*(px2-cx1) + (py2-cy1)*(py2-cy1) );

 float Ax, Ay;
 float Bx, By;
 float Cx, Cy;

 //find closest point to C  
 //printf("dist = %lf %lf\n", dist1, dist2);  

 Cx = cx1;
 Cy = cy1;
 if(dist1 < dist2)
 {
  Bx = px1;
  By = py1;
  Ax = px2;
  Ay = py2;


 }else{
  Bx = px2;
  By = py2;
  Ax = px1;
  Ay = py1;
 }


 float Q1 = Cx - Ax;
 float Q2 = Cy - Ay;
 float P1 = Bx - Ax;
 float P2 = By - Ay;


 float A = std::acos( (P1*Q1 + P2*Q2) / ( std::sqrt(P1*P1+P2*P2) * std::sqrt(Q1*Q1+Q2*Q2) ) );

 A = A*180/CV_PI;

 return A;
}

void CallbackFunc(int event, int x, int y, int flags, void* userdata)
{
  cv::Mat RGB = frame(cv::Rect(x, y, 1, 1));
  cv::Mat HSV;
  cv::cvtColor(RGB, HSV,  cv::COLOR_BGR2HSV);
  cv::Vec3b pixel = HSV.at<cv::Vec3b>(0, 0);
  if (event == cv::EVENT_LBUTTONDBLCLK) // on double left clcik
  {
      std::cout << "Click" << std::endl;
      int h = pixel.val[0];
      int s = pixel.val[1];
      int v = pixel.val[2];
      if (count == 0)
      {
          minH = h;
          maxH = h;
          minS = s;
          maxS = s;
          minV = v;
          maxV = v;
      }
      else
      {
          if (h < minH)
          {
              minH = h;
          }
          else if (h > maxH)
          {
              maxH = h;
          }
          if (s < minS)
          {
              minS = s;
          }
          else if (s > maxS)
          {
              maxS = s;
          }
          if (v < minV)
          {
              minV = v;
          }
          else if (v > maxV)
          {
              maxV = v;
          }
          
      }
      count++;
  }
  std::cout << pixel << std::endl;
}

int main()
{
  cv::VideoCapture cap(0);
  const char* windowName = "Fingertip detection";
  cv::namedWindow(windowName);
  cv::setMouseCallback(windowName, CallbackFunc, NULL);
  int inAngleMin = 200, inAngleMax = 300, angleMin = 180, angleMax = 359, lengthMin = 10, lengthMax = 80;
  cv::createTrackbar("Inner angle min", windowName, &inAngleMin, 360);
  cv::createTrackbar("Inner angle max", windowName, &inAngleMax, 360);
  cv::createTrackbar("Angle min", windowName, &angleMin, 360);
  cv::createTrackbar("Angle max", windowName, &angleMax, 360);
  cv::createTrackbar("Length min", windowName, &lengthMin, 100);
  cv::createTrackbar("Length max", windowName, &lengthMax, 100);
  while (1)
  {
      cap >> frame;
      cv::Mat hsv;
      cv::cvtColor(frame, hsv, cv::COLOR_BGR2HSV);
      cv::inRange(hsv, cv::Scalar(minH, minS, minV), cv::Scalar(maxH, maxS, maxV), hsv);
      // Pre processing
      int blurSize = 5;
      int elementSize = 5;
      cv::medianBlur(hsv, hsv, blurSize);
      cv::Mat element = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(2 * elementSize + 1, 2 * elementSize + 1), cv::Point(elementSize, elementSize));
      cv::dilate(hsv, hsv, element);
      // Contour detection
      std::vector<std::vector<cv::Point> > contours;
      std::vector<cv::Vec4i> hierarchy;
      cv::findContours(hsv, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
      size_t largestContour = 0;
      for (size_t i = 1; i < contours.size(); i++)
      {
          if (cv::contourArea(contours[i]) > cv::contourArea(contours[largestContour]))
              largestContour = i;
      }
      cv::drawContours(frame, contours, largestContour, cv::Scalar(0, 0, 255), 1);
      // Convex hull
      if (!contours.empty())
      {
          std::vector<std::vector<cv::Point> > hull(1);
          cv::convexHull(cv::Mat(contours[largestContour]), hull[0], false);
          cv::drawContours(frame, hull, 0, cv::Scalar(0, 255, 0), 3);
          if (hull[0].size() > 2)
          {
              std::vector<int> hullIndexes;
              cv::convexHull(cv::Mat(contours[largestContour]), hullIndexes, true);
              std::vector<cv::Vec4i> convexityDefects;
              cv::convexityDefects(cv::Mat(contours[largestContour]), hullIndexes, convexityDefects);
              cv::Rect boundingBox = cv::boundingRect(hull[0]);
              cv::rectangle(frame, boundingBox, cv::Scalar(255, 0, 0));
              cv::Point center = cv::Point(boundingBox.x + boundingBox.width / 2, boundingBox.y + boundingBox.height / 2);
              std::vector<cv::Point> validPoints;
              for (size_t i = 0; i < convexityDefects.size(); i++)
              {
                  cv::Point p1 = contours[largestContour][convexityDefects[i][0]];
                  cv::Point p2 = contours[largestContour][convexityDefects[i][1]];
                  cv::Point p3 = contours[largestContour][convexityDefects[i][2]];
                  double angle = std::atan2(center.y - p1.y, center.x - p1.x) * 180 / CV_PI;
                  double inAngle = innerAngle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
                  double length = std::sqrt(std::pow(p1.x - p3.x, 2) + std::pow(p1.y - p3.y, 2));
                  if (angle > angleMin - 180 && angle < angleMax - 180 && inAngle > inAngleMin - 180 && inAngle < inAngleMax - 180 && length > lengthMin / 100.0 * boundingBox.height && length < lengthMax / 100.0 * boundingBox.height)
                  {
                      validPoints.push_back(p1);
                  }
              }
              for (size_t i = 0; i < validPoints.size(); i++)
              {
                  cv::circle(frame, validPoints[i], 9, cv::Scalar(0, 255, 0), 2);
              }
          }
      }
      cv::imshow(windowName, frame);
      if (cv::waitKey(30) >= 0) break;
  }
  return 0;
}
