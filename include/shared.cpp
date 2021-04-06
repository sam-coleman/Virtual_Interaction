/**
 * Functions included in both Virtual Painting and Mouse Input
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
#include "shared.h"

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