#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdlib.h>
#include <tuple>

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
    int currColor = 0, currThick = 4;
    Mat controls;
    Mat video;
    Mat canvas;
    cap >> video;
    
    //make all mats the same size
    if(video.empty()) video = Mat::zeros(video.size(), video.type());
    resize(video, video, Size(), 1.5, 1.5); //increase canvas size by 150% from camera default
    if(controls.empty()) controls = Mat::zeros(video.size(), video.type());
    if(canvas.empty()) canvas = Mat::zeros(video.size(), video.type());
    cout << "video size is " << video.size() << '\n';
    //CREATE RECTANGLES FOR COLOR OPTIONS
    Rect clearControl(Point(0,1), Point(100,65));
    Rect blueControl(Point(200,1), Point(300,65));
    Rect greenControl( Point(400,1), Point(500,65));
    Rect redControl(Point(600,1), Point(700,65));
    Rect yellowControl(Point(800,1), Point(900,65));
    Rect currColorIndicator(Point (940, 1), Point(960, 20));
    //PLACE COLOR RECTANGLES ON CONTROL MAT
    rectangle(controls, currColorIndicator, getColorFromTuple(colors[currColor]), -1);
    rectangle(controls, clearControl, Scalar(255,255,255), -1);
    rectangle(controls, blueControl, Scalar(255,0,0), -1);
    rectangle(controls, greenControl, Scalar(0,255,0), -1);
    rectangle(controls, redControl, Scalar(0,0,255), -1);
    rectangle(controls, yellowControl, Scalar(0,255,255), -1);
    //ADD LABELS FOR COLOR RECTANGLES
    putText(controls, "CLEAR ALL", Point(10, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(controls, "BLUE", Point(225, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1.5, LINE_AA);
    putText(controls, "GREEN", Point(420, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1.5, LINE_AA);
    putText(controls, "RED", Point(630, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1.5, LINE_AA);
    putText(controls, "YELLOW", Point(820, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(150,150,150), 1.5, LINE_AA);

    //CREATE AND PLACE LINE THICKNESS RECTANGLES ON CONTROL MAT
    Rect _2thick(Point(920,80), Point(960, 150));
    Rect _4thick(Point(920,190), Point(960, 260));
    Rect _6thick(Point(920,300), Point(960,370));
    Rect _8thick(Point(920,410), Point(960,480));
    Rect _10thick(Point(920, 520), Point(960, 590));
    updateThickBoxes(controls, _2thick, _4thick, _6thick, _8thick, _10thick, currThick); //create rectangles first time

    Mat hsv;
    int x1 = 0, y1 = 0; //cordinates
    Rect boundBox; //bounding box for target
    int noiseThreshold = 500;

    for(;;) {
        cap >> video;
        resize(video, video, Size(), 1.5,1.5); //increase canvas size by 150% from camera default
        flip(video, video, +1); //mirror
        cvtColor(video, hsv, COLOR_BGR2HSV);
        rectangle(controls, currColorIndicator, getColorFromTuple(colors[currColor]), -1); //show current color in lower corner
        updateThickBoxes(controls, _2thick, _4thick, _6thick, _8thick, _10thick, currThick);

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
            else if ((blueControl & boundBox).area()> 0) { //switch color to blue
                currColor = 0;
            }
            else if ((greenControl & boundBox).area()>0) { //switch color to green
                currColor = 1;
            }
            else if ((redControl & boundBox).area()>0) { //switch color to red
                currColor = 2;
            }
            else if ((yellowControl & boundBox).area()>0) { //switch color to yellow
                currColor = 3;
            }
            else if ((clearControl & boundBox).area()>0) { //clear canvas
                canvas = Scalar(0,0,0);
            }
            else if ((_2thick & boundBox).area()>0) { //switch thickness to 2
                currThick = 2;
            }
            else if ((_4thick & boundBox).area()>0) { //switch thickness to 4
                currThick = 4;
            }
            else if ((_6thick & boundBox).area()>0) { //switch thickness to 6
                currThick = 6;
            }
            else if ((_8thick & boundBox).area()>0) { //switch thickness to 8
                currThick = 8;
            }
            else if ((_10thick & boundBox).area()>0) { //switch thickness to 10
                currThick = 10;
            }
            else {
                line(canvas, Point(x1, y1), Point(boundBox.x, boundBox.y), getColorFromTuple(colors[currColor]), currThick);
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