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

void updateThickBoxes(Mat mat, Rect _2, Rect _4, Rect _6, Rect _8, Rect _10, int currThick) {
    rectangle(mat, _2, Scalar(255,255,225), -1);
    rectangle(mat, _4, Scalar(255,255,255), -1);
    rectangle(mat, _6, Scalar(255,255,255), -1);
    rectangle(mat, _8, Scalar(255,255,255), -1);
    rectangle(mat, _10, Scalar(255,255,255), -1);
    putText(mat, "2", Point(940, 115), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(mat, "4", Point(940, 225), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(mat, "6", Point(940, 335), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(mat, "8", Point(940, 445), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(mat, "10", Point(920, 555), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);

    if (currThick == 2){
        rectangle(mat, _2, Scalar(0,255,0), -1);
        putText(mat, "2", Point(615, 97), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    }
    else if (currThick == 4){
        rectangle(mat, _4, Scalar(0,255,0), -1);
        putText(mat, "4", Point(615, 181), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    }
    else if (currThick == 6) {
        rectangle(mat, _6, Scalar(0,255,0), -1);
        putText(mat, "6", Point(615, 265), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    }
    else if (currThick == 8) {
        rectangle(mat, _8, Scalar(0,255,0), -1);
        putText(mat, "8", Point(615, 349), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    }
    else if (currThick == 10) {
        rectangle(mat, _10, Scalar(0,255,0), -1);
        putText(mat, "10", Point(610, 433), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);          
    }

}

int main(int argc, char** argv ) {

    VideoCapture cap;
    if (!cap.open(-1)) {
    return 0;
    }

    //Setup mouse window
    // Find display
    Display* dpy = XOpenDisplay(0);
    int scr = XDefaultScreen(dpy);
    Window root_window = XRootWindow(dpy, scr);

    // Determine display dimensions
    int screen_height = DisplayHeight(dpy, scr);
    int screen_width  = DisplayWidth(dpy, scr);
    std::cout << "Screen size : " << screen_width << "x" << screen_height << std::endl;

    // mouse coordinates
    int mouse_x;
    int mouse_y;

    Mat controls;
    Mat video;
    Mat canvas;
    cap >> video;

    //Find video size
    int video_height = video.size[1];
    int video_width = video.size[0];
    
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

            x1 = boundBox.x;
            y1 = boundBox.y;

            mouse_x = screen_width*x1/video_width;
            mouse_y = screen_height*y1/video_height;

            // Move pointer to coords (mouse_x,mouse_y)
            XWarpPointer(dpy, None, root_window, 0, 0, 0, 0, mouse_x, mouse_y);
            XFlush(dpy);
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