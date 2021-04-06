/**
 * Functions used in Virtual Painting
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
#include "painting.h"

using namespace cv;
using namespace std;

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
void UpdateThickBoxes(Mat mat, Rect _4, Rect _8, Rect _12, Rect _20, int currThick) {
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