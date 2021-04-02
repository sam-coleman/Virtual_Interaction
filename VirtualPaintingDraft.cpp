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

void updateThickBoxes(Mat mat, Rect _4, Rect _8, Rect _12, Rect _20, int currThick) {
    rectangle(mat, _4, Scalar(255,255,255), -1);
    rectangle(mat, _8, Scalar(255,255,255), -1);
    rectangle(mat, _12, Scalar(255,255,255), -1);
    rectangle(mat, _20, Scalar(255,255,255), -1);

    putText(mat, "4", Point(940, 130), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(mat, "8", Point(940, 280), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(mat, "12", Point(940, 430), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(mat, "20", Point(940, 600), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);

    if (currThick == 4){
        rectangle(mat, _4, Scalar(0,255,0), -1);
        putText(mat, "4", Point(940, 130), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);

    }
    else if (currThick == 8) {
        rectangle(mat, _8, Scalar(0,255,0), -1);
        putText(mat, "8", Point(940, 280), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    }
    else if (currThick == 12) {
        rectangle(mat, _12, Scalar(0,255,0), -1);
        putText(mat, "12", Point(940, 430), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    }
    else if (currThick == 20) {
        rectangle(mat, _20, Scalar(0,255,0), -1);
        putText(mat, "20", Point(940, 600), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);          
    }

}

int main(int argc, char** argv ) {

    //CHANGE LOWER AND UPPER BOUND OF COLOR TARGET HERE
    Scalar lower_bound = Scalar(125, 122, 99); //lower bound HSV of color target
    Scalar upper_bound = Scalar(179, 255, 255); //upper bound HSV of color target

    VideoCapture cap;
    if (!cap.open(-1)) {
    return 0;
    }

    //Colors are BGR
    //order of colors in vector: blue, green, red, yellow
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
    
    //currColor: 0=blue, 1=green, 2=red, 3=yellow
    int currColor = 0, currThick = 4;
    Mat controls;
    Mat video;
    Mat canvas;
    cap >> video;
    
    //MAKE ALL MATS THE SAME SIZE
    if(video.empty()) video = Mat::zeros(video.size(), video.type());
    resize(video, video, Size(), 1.5, 1.5); //increase canvas size by 150% from camera default
    if(controls.empty()) controls = Mat::zeros(video.size(), video.type());
    if(canvas.empty()) canvas = Mat::zeros(video.size(), video.type());
    cout << "video size is " << video.size() << '\n';
    
    //CREATE RECTANGLES FOR COLOR OPTIONS
    Rect clearControl(Point(0,0), Point(100,65));
    Rect whiteControl(Point(150,0), Point(250,65));
    Rect blueControl( Point(300,0), Point(400,65));
    Rect greenControl(Point(450,0), Point(550,65));
    Rect redControl(Point(600,0), Point(700,65));
    Rect yellowControl(Point(750,0), Point(850,65));
    Rect currColorIndicator(Point (940,0), Point(960, 20));
    
    //PLACE COLOR RECTANGLES ON CONTROL MAT
    rectangle(controls, clearControl, Scalar(255,255,255), -1);
    rectangle(controls, whiteControl, Scalar(255,255,255), -1);
    rectangle(controls, blueControl, Scalar(255,0,0), -1);
    rectangle(controls, greenControl, Scalar(0,255,0), -1);
    rectangle(controls, redControl, Scalar(0,0,255), -1);
    rectangle(controls, yellowControl, Scalar(0,255,255), -1);
    rectangle(controls, currColorIndicator, getColorFromTuple(colors[currColor]), -1);

    //ADD LABELS FOR COLOR RECTANGLES
    putText(controls, "CLEAR ALL", Point(10, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(controls, "WHITE", Point(170, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(controls, "BLUE", Point(320, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1.5, LINE_AA);
    putText(controls, "GREEN", Point(470, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1.5, LINE_AA);
    putText(controls, "RED", Point(620, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1.5, LINE_AA);
    putText(controls, "YELLOW", Point(770, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(150,150,150), 1.5, LINE_AA);

    //CREATE AND PLACE LINE THICKNESS RECTANGLES ON CONTROL MAT
    Rect _4thick(Point(920, 80), Point(960, 180));
    Rect _8thick(Point(920, 230), Point(960, 330));
    Rect _12thick(Point(920, 380), Point(960, 480));
    Rect _20thick(Point(920, 530), Point(960, 630));
    updateThickBoxes(controls, _4thick, _8thick, _12thick, _20thick, currThick); //create rectangles first time

    Mat hsv;
    int x1 = 0, y1 = 0; //cordinates
    Rect boundBox; //bounding box for target
    int noiseThreshold = 500; //noise threshold

    for(;;) {
        cap >> video;
        resize(video, video, Size(), 1.5,1.5); //increase canvas size by 150% from camera default
        flip(video, video, +1); //mirror
        cvtColor(video, hsv, COLOR_BGR2HSV);
        rectangle(controls, currColorIndicator, getColorFromTuple(colors[currColor]), -1); //show current color in lower corner
        updateThickBoxes(controls, _4thick, _8thick, _12thick, _20thick, currThick);

        //MORPHING TECHNIQUES
        Mat kernal = Mat(5, 5, CV_8U, Scalar(1,1,1));
        Mat mask;
        inRange(hsv, lower_bound, upper_bound, mask); //Define color range of target
        erode(mask, mask, kernal);
        erode(mask, mask, kernal);
        morphologyEx(mask, mask, MORPH_OPEN, kernal);
        dilate(mask, mask, kernal);

        //FIND CONTOURS
        vector <vector<Point> > contours;
        findContours(mask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
        if (contours.size() >= 1 && contourArea(contours.at(getMaxAreaContourId(contours))) > noiseThreshold) { //if sees color target
            vector<Point> c = contours.at(getMaxAreaContourId(contours));
            boundBox = boundingRect(c);

            if (x1 == 0 && y1 == 0) {
                x1, y1 = boundBox.x, boundBox.y;
            } 
            else if ((whiteControl & boundBox).area() > 0) { //switch color to white
                currColor = 0;
            }           
            else if ((blueControl & boundBox).area()> 0) { //switch color to blue
                currColor = 1;
            }
            else if ((greenControl & boundBox).area()>0) { //switch color to green
                currColor = 2;
            }
            else if ((redControl & boundBox).area()>0) { //switch color to red
                currColor = 3;
            }
            else if ((yellowControl & boundBox).area()>0) { //switch color to yellow
                currColor = 4;
            }
            else if ((clearControl & boundBox).area()>0) { //clear canvas
                canvas = Scalar(0,0,0);
            }
            else if ((_4thick & boundBox).area()>0) { //switch thickness to 2
                currThick = 4;
            }
            else if ((_8thick & boundBox).area()>0) { //switch thickness to 4
                currThick = 8;
            }
            else if ((_12thick & boundBox).area()>0) { //switch thickness to 6
                currThick = 12;
            }
            else if ((_20thick & boundBox).area()>0) { //switch thickness to 8
                currThick = 20;
            }
            else { //draw line
                line(canvas, Point(x1, y1), Point(boundBox.x, boundBox.y), getColorFromTuple(colors[currColor]), currThick);
            }
            //update current coordinates
            x1 = boundBox.x;
            y1 = boundBox.y;
        } 
        else { //reset coordinates
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