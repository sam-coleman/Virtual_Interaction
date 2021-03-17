#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdlib.h>
#include <tuple>

//https://medium.com/programming-fever/draw-using-a-virtual-pen-on-a-computer-screen-using-opencv-in-python-71d3a1d5902b

using namespace cv;
using namespace std;

int getMaxAreaContourId(vector <vector<Point>> contours) {
        //Soure: https://stackoverflow.com/questions/46187563/finding-largest-contours-c
        double maxArea = 0;
        int maxAreaContourId = -1;
        for (int j = 0; j < contours.size(); j++) {
            double newArea = contourArea(contours.at(j));
            if (newArea > maxArea) {
                maxArea = newArea;
                maxAreaContourId = j;
            }
        }
    return maxAreaContourId;
}

Scalar getColorFromTuple (tuple<int, int, int> color) {
    //take in tuple, return Scalar color
    int blue, green, red;
    blue = get<0>(color);
    green = get<1>(color);
    red = get<2>(color);
    return Scalar(blue, green, red);
}

int main(int argc, char** argv ) {

    VideoCapture cap;
    if (!cap.open(-1)) {
    //if(!cap.open(-1)) {
    return 0;
    //}
    }

    //DEQUEUE FUNCTION
    //isn't used right now
    // deque<int> bpoints;
    // bpoints.max_size();
    // deque<int> gpoints;
    // gpoints.max_size();
    // deque<int> rpoints;
    // rpoints.max_size();
    // deque<int> ypoints;
    // ypoints.max_size();
    // int bindex=0, gindex=0, rindex=0, yindex=0, colorIndex = 0; 
  
    //Colors are BGR
    //order of colors in vector: blue, green, red, yellow
    tuple <int, int, int> blue;
    blue = make_tuple(255, 0, 0);
    tuple <int, int, int> green;
    green = make_tuple(0, 255, 0);
    tuple <int, int, int> red;
    red = make_tuple(0, 0, 255);
    tuple <int, int, int> yellow;
    yellow = make_tuple(0, 255, 255);
    vector <tuple<int, int, int>> colors;
    colors.push_back(blue);
    colors.push_back(green);
    colors.push_back(red);
    colors.push_back(yellow);

    //currColor: 0=blue, 1=green, 2=red, 3=yellow
    int currColor = 0;
    Mat controls;
    Mat video;
    Mat canvas;
    cap >> video;
    if(video.empty()) video = Mat::zeros(video.size(), video.type());
    if(controls.empty()) controls = Mat::zeros(video.size(), video.type());
    if(canvas.empty()) canvas = Mat::zeros(video.size(), video.type());
    
    //CREATE RECTANGLES FOR COLOR OPTIONS
    Rect clearControl(Point(40,1), Point(140,65));
    Rect blueControl(Point(160,1), Point(255,65));
    Rect greenControl( Point(275,1), Point(370,65));
    Rect redControl(Point(390,1), Point(485,65));
    Rect yellowControl(Point(505,1), Point(600,65));
    Rect currColorIndicator(Point (581, 460), Point(600, 479));
    //PLACE RECTANGLES ON CONTROL MAT
    rectangle(controls, clearControl, Scalar(255,255,255), -1);
    rectangle(controls, blueControl, Scalar(255,0,0), -1);
    rectangle(controls, greenControl, Scalar(0,255,0), -1);
    rectangle(controls, redControl, Scalar(0,0,255), -1);
    rectangle(controls, yellowControl, Scalar(0,255,255), -1);
    putText(controls, "CLEAR ALL", Point(49, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(controls, "BLUE", Point(183, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1.5, LINE_AA);
    putText(controls, "GREEN", Point(298, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1.5, LINE_AA);
    putText(controls, "RED", Point(420, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1.5, LINE_AA);
    putText(controls, "YELLOW", Point(520, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(150,150,150), 1.5, LINE_AA);

    rectangle(controls, currColorIndicator, getColorFromTuple(colors[currColor]), -1);
    cout << "video size is " << video.size() << '\n';
    Mat hsv;
    int x1 = 0, y1 = 0; //cordinates
    Rect boundBox; //bounding box for target
    int noiseThreshold = 500;

    for(;;) {
        cap >> video;
        flip(video, video, +1);
        cvtColor(video, hsv, COLOR_BGR2HSV);
        rectangle(controls, currColorIndicator, getColorFromTuple(colors[currColor]), -1); //show current color in lower corner

        //MORPHING TECHNIQUES
        Mat kernal = Mat(5, 5, CV_8U, Scalar(1,1,1));
        Mat mask;
        inRange(hsv, Scalar(130, 126, 75), Scalar(179, 255, 255), mask);
        erode(mask, mask, kernal);
        erode(mask, mask, kernal);
        morphologyEx(mask, mask, MORPH_OPEN, kernal);
        dilate(mask, mask, kernal);
        vector <vector<Point> > contours;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        if (contours.size() >= 1 && contourArea(contours.at(getMaxAreaContourId(contours))) > noiseThreshold) {
            vector<Point> c = contours.at(getMaxAreaContourId(contours));
            boundBox = boundingRect(c);

            if (x1 == 0 && y1 == 0) {
                x1, y1 = boundBox.x, boundBox.y;
            }            
            else if ((blueControl & boundBox).area()> 0) {
                currColor = 0;
            }
            else if ((greenControl & boundBox).area()>0) {
                currColor = 1;
            }
            else if ((redControl & boundBox).area()>0) {
                currColor = 2;
            }
            else if ((yellowControl & boundBox).area()>0) {
                currColor = 3;
            }
            else if ((clearControl & boundBox).area()>0) {
                canvas = Scalar(0,0,0);
            }
            else {
                line(canvas, Point(x1, y1), Point(boundBox.x, boundBox.y), getColorFromTuple(colors[currColor]), 4);
            }
            x1 = boundBox.x;
            y1 = boundBox.y;
        } 
        else {
            x1 = 0;
            y1 = 0;
        }
    
        if (canvas.empty()) break; //end of video stream
        imshow("video in",  controls + video + canvas);
        imshow("canvas", controls + canvas);
        if (waitKey(10) == 27) break; //stop by ESC key
    }
    return 0;
}