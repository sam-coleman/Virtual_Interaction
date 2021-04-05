#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdlib.h>
#include <tuple>

using namespace cv;
using namespace std;

/**
* Determines the maximum contour from a vector of contours.
*
* Source: https://stackoverflow.com/questions/46187563/finding-largest-contours-c
*
* @param <contours> Vector containing all the contours from the color target.
* @return The index of the maximum contour.
*/
int GetMaxAreaContourId(vector <vector<Point>> contours) {
        //initialize variables
        double maxArea = 0;
        int maxAreaContourId = -1;
        //iterate through all contours
        for (int j = 0; j < contours.size(); j++) {
            double newArea = contourArea(contours.at(j)); //determine area of contour
            if (newArea > maxArea) { //update maximum contour
                maxArea = newArea;
                maxAreaContourId = j;
            }
        }
    return maxAreaContourId;
}

/**
* Convert color represented as BGR tuple to a Scalar type.
*
* @param <color> Tuple representing a color of type BGR.
* @return Scalar representing the color.
*/
Scalar GetColorFromTuple (tuple<int, int, int> color) {
    int blue, green, red;
    blue = get<0>(color);
    green = get<1>(color);
    red = get<2>(color);
    return Scalar(blue, green, red);
}

/**
* Update and redraw the thickness control rectangles on the screen.
*
* @param <mat> Mat object to draw on.
* @param <_4> Rectangle representing thickness of 4.
* @param <_8> Rectangle representing thickness of 8.
* @param <_12> Rectangle representing thickness of 12.
* @param <_20> Rectangle representing thickness of 20.
* @param <currThick> Current thickness of paintbrush.
*/
void updateThickBoxes(Mat mat, Rect _4, Rect _8, Rect _12, Rect _20, int currThick) {
    // reset all rectangles to white and put text
    rectangle(mat, _4, Scalar(255,255,255), -1);
    rectangle(mat, _8, Scalar(255,255,255), -1);
    rectangle(mat, _12, Scalar(255,255,255), -1);
    rectangle(mat, _20, Scalar(255,255,255), -1);
    putText(mat, "4", Point(940, 130), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(mat, "8", Point(940, 280), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(mat, "12", Point(940, 430), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(mat, "20", Point(940, 600), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);

    //switch to make rectangle of current thickness green to indicate to user it is selected
    switch (currThick) {
        case 4:
            rectangle(mat, _4, Scalar(0,255,0), -1);
            putText(mat, "4", Point(940, 130), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
            break;
        case 8:
            rectangle(mat, _8, Scalar(0,255,0), -1);
            putText(mat, "8", Point(940, 280), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
            break;
        case 12:
            rectangle(mat, _12, Scalar(0,255,0), -1);
            putText(mat, "12", Point(940, 430), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
            break;
        case 20:
            rectangle(mat, _20, Scalar(0,255,0), -1);
            putText(mat, "20", Point(940, 600), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
            break;          
    }
}

/**
* Draw the controls for clear and color on the mat.
*
* @param <mat> Mat object to draw on.
* @param <clearControl> Rectangle to clear drawing.
* @param <whiteControl> Rectangle to change color to white.
* @param <blueControl> Rectangle to change color to blue.
* @param <greenControl> Rectangle to change color to green.
* @param <redControl> Rectangle to change color to red.
* @param <yellowControl> Rectangle to change color to yellow.
* @param <currColorIndicator> Rectangle to display the current color.
* @param <colors> Vector of tuples representing all available colors with their BGR values.
*/
void InitControls(Mat mat, Rect clearControl, Rect whiteControl, Rect blueControl, Rect greenControl,
                  Rect redControl, Rect yellowControl, Rect currColorIndicator, vector<tuple<int, int, int>> colors) {
    
    //place rectangles on mat
    rectangle(mat, clearControl, Scalar(255,255,255), -1);
    rectangle(mat, whiteControl, Scalar(255,255,255), -1);
    rectangle(mat, blueControl, Scalar(255,0,0), -1);
    rectangle(mat, greenControl, Scalar(0,255,0), -1);
    rectangle(mat, redControl, Scalar(0,0,255), -1);
    rectangle(mat, yellowControl, Scalar(0,255,255), -1);
    rectangle(mat, currColorIndicator, GetColorFromTuple(colors[0]), -1);

    //add rectangle labels
    putText(mat, "CLEAR ALL", Point(10, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(mat, "WHITE", Point(170, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1.5, LINE_AA);
    putText(mat, "BLUE", Point(320, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1.5, LINE_AA);
    putText(mat, "GREEN", Point(470, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1.5, LINE_AA);
    putText(mat, "RED", Point(620, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1.5, LINE_AA);
    putText(mat, "YELLOW", Point(770, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(150,150,150), 1.5, LINE_AA);
}

int main(int argc, char** argv ) {

    //Color target defaults to red unless user inputs lower and upper bounds
    Scalar lower_bound = Scalar(125, 122, 99); //lower bound HSV of color target
    Scalar upper_bound = Scalar(179, 255, 255); //upper bound HSV of color target

    //if user provides inputs, those become lower and upper bound
    if (argc == 7) {
        lower_bound = Scalar(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]));
        upper_bound = Scalar(atoi(argv[4]), atoi(argv[5]), atoi(argv[6]));
    }

    //start video capture
    VideoCapture cap;
    if (!cap.open(-1)) {
    return 0;
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
 
    //CREATE AND PLACE LINE THICKNESS RECTANGLES ON CONTROL MAT
    Rect _4thick(Point(920, 80), Point(960, 180));
    Rect _8thick(Point(920, 230), Point(960, 330));
    Rect _12thick(Point(920, 380), Point(960, 480));
    Rect _20thick(Point(920, 530), Point(960, 630));
    updateThickBoxes(controls, _4thick, _8thick, _12thick, _20thick, currThick); //create rectangles first time

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
        updateThickBoxes(controls, _4thick, _8thick, _12thick, _20thick, currThick); //update thickness boxes to indicate current thickness

        //Perform morphing techniques
        Mat kernal = Mat(5, 5, CV_8U, Scalar(1,1,1));
        Mat mask;
        inRange(hsv, lower_bound, upper_bound, mask);
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
            else if ((_4thick & boundBox).area()>0) { //switch thickness to 4
                currThick = 4;
            }
            else if ((_8thick & boundBox).area()>0) { //switch thickness to 8
                currThick = 8;
            }
            else if ((_12thick & boundBox).area()>0) { //switch thickness to 12
                currThick = 12;
            }
            else if ((_20thick & boundBox).area()>0) { //switch thickness to 20
                currThick = 20;
            }
            else { //draw line
                line(canvas, Point(x1, y1), Point(boundBox.x, boundBox.y), GetColorFromTuple(colors[currColor]), currThick);
            }

            //update current coordinates
            x1 = boundBox.x;
            y1 = boundBox.y;
        } 
        else { //reset coordinates if target not seen
            x1 = 0;
            y1 = 0;
        }
    
        if (canvas.empty()) break; //end of video stream

        //show windoes
        imshow("video in",  controls + video + canvas); //overlay video feed, controls, and canvas
        imshow("canvas", controls + canvas); //overlay controls and canvas
        
        int key_pressed = waitKey(10); //wait to see if key pressed

        if (key_pressed == 27) break; //stop by ESC key

        else if (key_pressed == 115) { //save mat by hitting s 
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