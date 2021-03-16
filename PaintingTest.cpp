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

int main(int argc, char** argv ) {

    VideoCapture cap;
    if (!cap.open(-1)) {
    //if(!cap.open(-1)) {
    return 0;
    //}
    }

    //DEQUEUE FUNCTION
    deque<int> bpoints;
    bpoints.max_size();
    deque<int> gpoints;
    gpoints.max_size();
    deque<int> rpoints;
    rpoints.max_size();
    deque<int> ypoints;
    ypoints.max_size();
    int bindex=0, gindex=0, rindex=0, yindex=0, colorIndex = 0; 
    //Colors are BGR
    tuple <int, int, int> blue;
    tuple <int, int, int> green;
    tuple <int, int, int> red;
    tuple <int, int, int> yellow;
    blue = make_tuple(255, 0, 0);
    //can we make an array of tuples??

    Mat Window;
    Mat video;
    cap >> video;
    if(Window.empty()) Window = cv::Mat::zeros(video.size(), video.type());

    //CREATE RECTANGLES FOR COLOR OPTIONS
    //TODO: get Scalar value from color (replace Sclar(#,#,#)) with tuples defined above
    rectangle(Window, Point(40,1), Point(140,65), Scalar(255,255,255), -1);
    rectangle(Window, Point(160,1), Point(255,65), Scalar(255,0,0), -1);
    rectangle(Window, Point(275,1), Point(370,65), Scalar(0,255,0), -1);
    rectangle(Window, Point(390,1), Point(485,65), Scalar(0,0,255), -1);
    rectangle(Window, Point(505,1), Point(600,65), Scalar(0,255,255), -1);
    putText(Window, "CLEAR ALL", Point(49, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1, LINE_AA);
    putText(Window, "BLUE", Point(185, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1, LINE_AA);
    putText(Window, "GREEN", Point(298, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1, LINE_AA);
    putText(Window, "RED", Point(420, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1, LINE_AA);
    double alpha = .5, gamma = 0;
    Mat hsv;
    int x1 = 0, y1 = 0; //cordinates
    Rect boundBox;

    for(;;) {
        cap >> video;
        flip(video, video, +1);
        cvtColor(video, hsv, COLOR_BGR2HSV);
        //cvtColor(video, video, COLOR_BGR2HSV);
        //Mat combined;
        //addWeighted(Window, alpha, video, 1-alpha, gamma, combined);

        //MORPHING TECHNIQUES
        Mat kernal = Mat(5, 5, CV_8U, Scalar(1,1,1));
        //InputArray kernal();
        // for (int i = 0; i < 5; i++) {
        //     for (int j = 0; j < 0; j++) {
        //         kernal[i][j] = 1;
        //     }
        // }
        Mat mask;
        inRange(hsv, Scalar (161, 155, 84), Scalar (179, 255, 255), mask);
        erode(mask, mask, kernal); //iterations = 2);
        morphologyEx(mask, mask, MORPH_OPEN, kernal);
        dilate(mask, mask, kernal);
        vector <vector<Point> > contours;
        //vector<Vec4i> hierarchy;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        //TODO: DEAL WITH NOISE
        if (contours.size() >= 1) {
        //TODO: make into separate function 
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
        vector<Point> c = contours.at(maxAreaContourId);
        boundBox = boundingRect(c);

         if (x1 == 0 && y1 == 0) {
            x1, y1 = boundBox.x, boundBox.y;
        }
        else {
            line(Window, Point(x1, y1), Point(boundBox.x, boundBox.y), Scalar(255, 0, 0), 4);
            x1 = boundBox.x;
            y1 = boundBox.y;
        }
        } 
        else {
            x1 = 0;
            y1 = 0;
 
        }
        
        // if (x1 == 0 && y1 == 0) {
        //     x1, y1 = boundBox.x, boundBox.y;
        // }
        // else {
        //     line(Window, Point(x1, y1), Point(boundBox.x, boundBox.y), Scalar(255, 0, 0), 4);
        //     x1 = boundBox.x;
        //     y1 = boundBox.y;
        // }
        //update points: new points become previous points


        //contourArea(max(cnts, key = contourArea)) > 800; //TODO: noise threshold
        //findContours(mask, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        //cout << "size is " << cnts.size() << '\n';

        //CREATE BOUNDING CIRCLE
        // if (cnts.size() > 0) {
        //     vector <vector<Point>> cnt;
        //     sort(cnts, cnt); //TODO: THINK THIS IS WRONG
        // }

        if (Window.empty()) break; //end of video stream
        //imshow("video in", combined);
        imshow("video in", video + Window);
        if (waitKey(10) == 27) break; //stop by ESC key
    }
    return 0;
}