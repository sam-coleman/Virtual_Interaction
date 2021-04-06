/**
 * Virtual Move Mouse
 * A program that detects and tracks a color object to move your mouse
 * 
 * @author Sam Coleman
 * @author Kate Mackowiak
 */

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
#include "include/shared.h"
using namespace cv;
using namespace std;

int main(int argc, char** argv ) {

    //start video capture
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

    Mat video;
    cap >> video;

    //Find video size
    int videoHeight = video.size[0];
    int videoWidth = video.size[1];
    
    //Resize video mat
    if(video.empty()) video = Mat::zeros(video.size(), video.type());
    resize(video, video, Size(), 1.5, 1.5); //increase canvas size by 150% from camera default

    Mat hsv;
    int x1 = 0, y1 = 0; //cordinates
    Rect boundBox; //bounding box for target
    int noiseThreshold = 500;

    for(;;) {

        //update capture to video and perform basic operations
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

        if (contours.size() >= 1 && contourArea(contours.at(GetMaxAreaContourId(contours))) > noiseThreshold) {
            vector<Point> c = contours.at(GetMaxAreaContourId(contours));
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
    
        if (video.empty()) break; //end of video stream
        imshow("video in", video);

        int keyPressed = waitKey(10); //wait to see if key pressed

        if (keyPressed == 27) break; //stop by ESC key
    }
    return 0;
}