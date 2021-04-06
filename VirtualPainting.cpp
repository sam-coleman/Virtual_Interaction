/**
 * Virtual Painting 
 * A program that detects and tracks a color object to paint on the display.
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
#include "include/painting.h"
#include "include/shared.h"

using namespace cv;
using namespace std;

int main(int argc, char** argv ) {
    //Color target defaults to red unless user inputs lower and upper bounds
    Scalar lowerBound = Scalar(125, 122, 99); //lower bound HSV of color target
    Scalar upperBound = Scalar(179, 255, 255); //upper bound HSV of color target

    //if user provides inputs, update lower and upper bounds
    if (argc == 7) {
        lowerBound = Scalar(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
        upperBound = Scalar(atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
    }

    //define colors and put them in vector holding all colors
    //order of colors in vector: white, blue, green, red, yellow
    tuple <int, int, int> white;
    white = make_tuple(255,255,255);
    tuple <int, int, int> blue;
    blue = make_tuple(255, 0, 0);
    tuple <int, int, int> green;
    green = make_tuple(0, 255, 0);
    tuple <int, int, int> red;
    red = make_tuple(0, 0, 255);
    tuple <int, int, int> yellow;
    yellow = make_tuple(0, 255, 255);
    vector <tuple<int, int, int>> colors;
    colors.push_back(white);
    colors.push_back(blue);
    colors.push_back(green);
    colors.push_back(red);
    colors.push_back(yellow);
    
    int currColor = 0, currThick = 4; //initialize color to white and thickness to 4
    
    //start video capture
    VideoCapture cap;
    if (!cap.open(-1)) {
    return 0;
    }

    //create mats and make all the same size
    Mat controls;
    Mat video;
    Mat canvas;
    cap >> video;
    if(video.empty()) video = Mat::zeros(video.size(), video.type());
    resize(video, video, Size(), 1.5, 1.5); //increase canvas size by 150% from camera default
    if(controls.empty()) controls = Mat::zeros(video.size(), video.type());
    if(canvas.empty()) canvas = Mat::zeros(video.size(), video.type());
    
    //create rectangles for clear, color options, and current color indicator
    Rect clearControl(Point(0,0), Point(100,65));
    Rect whiteControl(Point(150,0), Point(250,65));
    Rect blueControl( Point(300,0), Point(400,65));
    Rect greenControl(Point(450,0), Point(550,65));
    Rect redControl(Point(600,0), Point(700,65));
    Rect yellowControl(Point(750,0), Point(850,65));
    Rect currColorIndicator(Point (940,0), Point(960, 20));
    InitControls(controls, clearControl, whiteControl, blueControl, greenControl, redControl, yellowControl, currColorIndicator, colors);
 
    //create rectangles for line thicknesses
    Rect _4thick(Point(920, 80), Point(960, 180));
    Rect _8thick(Point(920, 230), Point(960, 330));
    Rect _12thick(Point(920, 380), Point(960, 480));
    Rect _20thick(Point(920, 530), Point(960, 630));
    UpdateThickBoxes(controls, _4thick, _8thick, _12thick, _20thick, currThick);

    Mat hsv; //mat to convert to HSV values
    int x1 = 0, y1 = 0; //cordinates of target
    Rect boundBox; //bounding box for target
    int noiseThreshold = 500; //noise threshold

    for(;;) {
        //update capture to video and perform basic operations
        cap >> video;
        resize(video, video, Size(), 1.5,1.5); //increase canvas size by 150% from camera default
        flip(video, video, +1); //mirror
        cvtColor(video, hsv, COLOR_BGR2HSV);

        rectangle(controls, currColorIndicator, GetColorFromTuple(colors[currColor]), -1); //show current color in lower corner
        UpdateThickBoxes(controls, _4thick, _8thick, _12thick, _20thick, currThick); //update thickness boxes to indicate current thickness

        //Perform morphing techniques
        Mat kernal = Mat(5, 5, CV_8U, Scalar(1,1,1));
        Mat mask;
        inRange(hsv, lowerBound, upperBound, mask);
        erode(mask, mask, kernal);
        erode(mask, mask, kernal);
        morphologyEx(mask, mask, MORPH_OPEN, kernal);
        dilate(mask, mask, kernal);

        //find contours
        vector <vector<Point> > contours;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        //if color target is seen
        if (contours.size() >= 1 && contourArea(contours.at(GetMaxAreaContourId(contours))) > noiseThreshold) { //if sees color target
            vector<Point> c = contours.at(GetMaxAreaContourId(contours)); //location of color target
            boundBox = boundingRect(c); //create bounding box

            //Perform operation depending on where target is: update color, thickness, clear, or draw line
            if (x1 == 0 && y1 == 0) {
                x1, y1 = boundBox.x, boundBox.y;
            } else if ((whiteControl & boundBox).area() > 0) { //switch color to white
                currColor = 0;
            } else if ((blueControl & boundBox).area()> 0) { //switch color to blue
                currColor = 1;
            } else if ((greenControl & boundBox).area()>0) { //switch color to green
                currColor = 2;
            } else if ((redControl & boundBox).area()>0) { //switch color to red
                currColor = 3;
            } else if ((yellowControl & boundBox).area()>0) { //switch color to yellow
                currColor = 4;
            } else if ((clearControl & boundBox).area()>0) { //clear canvas
                canvas = Scalar(0,0,0);
            } else if ((_4thick & boundBox).area()>0) { //switch thickness to 4
                currThick = 4;
            } else if ((_8thick & boundBox).area()>0) { //switch thickness to 8
                currThick = 8;
            } else if ((_12thick & boundBox).area()>0) { //switch thickness to 12
                currThick = 12;
            } else if ((_20thick & boundBox).area()>0) { //switch thickness to 20
                currThick = 20;
            } else { //draw line
                line(canvas, Point(x1, y1), Point(boundBox.x, boundBox.y), GetColorFromTuple(colors[currColor]), currThick);
            }
            //update current coordinates
            x1 = boundBox.x;
            y1 = boundBox.y;
        } else { //reset coordinates if target not seen
            x1 = 0;
            y1 = 0;
        }
    
        if (canvas.empty()) break; //end of video stream

        //show windows
        imshow("video in",  controls + video + canvas); //overlay video feed, controls, and canvas
        imshow("canvas", controls + canvas); //overlay controls and canvas
        
        int keyPressed = waitKey(10); //wait to see if key pressed

        if (keyPressed == 27) break; //stop by ESC key

        else if (keyPressed == 115) { //save mat by hitting s 
            //save image with name from user in saved_outputs folder
            cout << "Please enter the image name (including the .extension): ";
            String fileName;
            getline(cin, fileName);
            stringstream filePath;
            filePath << ".." << "/" << "saved_outputs" << "/" << fileName;
            imwrite(filePath.str(), canvas);
            cout << '\n';
        }
    }
    return 0;
}