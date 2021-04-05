#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdlib.h>
#include <tuple>
#include <X11/Xlib.h>
#include <iostream>
#include <unistd.h>

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
    return 0;
    }

    //Setup mouse window
    // Find display
    Display* display = XOpenDisplay(0);
    int scr = XDefaultScreen(display);
    Window rootWindow = XRootWindow(display, scr);
    // XEvent event;

    // Determine display dimensions
    int screenHeight = DisplayHeight(display, scr);
    int screenWidth  = DisplayWidth(display, scr);

    // mouse coordinates
    int mouseX;
    int mouseY;

    Mat controls;
    Mat video;
    Mat canvas;
    cap >> video;

    //Find video size
    int videoHeight = video.size[1];
    int videoWidth = video.size[0];
    
    //make all mats the same size
    if(video.empty()) video = Mat::zeros(video.size(), video.type());
    resize(video, video, Size(), 1.5, 1.5); //increase canvas size by 150% from camera default
    if(controls.empty()) controls = Mat::zeros(video.size(), video.type());
    if(canvas.empty()) canvas = Mat::zeros(video.size(), video.type());
    cout << "video size is " << video.size() << '\n';

    Mat hsv;
    int x1 = 0, y1 = 0; //cordinates
    Rect boundBox; //bounding box for target
    int noiseThreshold = 500;

    for(;;) {
        cap >> video;
        resize(video, video, Size(), 1.5,1.5); //increase canvas size by 150% from camera default
        flip(video, video, +1); //mirror
        cvtColor(video, hsv, COLOR_BGR2HSV);

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

            // get contour coordinates
            x1 = boundBox.x;
            y1 = boundBox.y;

            // scale mouse coordinates
            mouseX = screenWidth*x1/videoWidth;
            mouseY = screenHeight*y1/videoHeight;

            // Move pointer to coords (mouseX,mouseY)
            XWarpPointer(display, None, rootWindow, 0, 0, 0, 0, mouseX, mouseY);
            XFlush(display);
        } 
        else {
            x1 = 0;
            y1 = 0;
        }
    
        if (canvas.empty()) break; //end of video stream
        imshow("video in",  controls + video + canvas);
        imshow("canvas", controls + canvas);
        if (waitKey(10) == 27) break; //stop by ESC key

        if (waitKey(10) == 115) { //save mat by hitting s 
            //save location is in saved_outputs folder
            cout << "Please enter the image name (including the .extension): ";
            String file_name;
            getline(cin, file_name);
            stringstream file_path;
            file_path << ".." << "/" << "saved_outputs" << "/" << file_name;
            imwrite(file_path.str(), canvas);
            cout << '\n';
        }
    }
    return 0;
}