#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;
int main(int argc, char** argv )
{
    
    //Get Camera feed in and display
    VideoCapture cap;
    // use default camera
    if (!cap.open(0)) {
        return 0;
    }
    for (;;) { //while loop
        Mat frame;
        cap >> frame;
        if (frame.empty()) break; //end of video stream
        imshow("video in", frame);
        if (waitKey(10) == 27) break; //stop by ESC key
    }

    return 0;
}
