#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

using namespace cv;
int main(int argc, char** argv )
{

    //Get Camera feed in and display
    VideoCapture cap;
    // use default camera
    if (!cap.open(0)) {
        return 0;
    }

    Point p1(0,0), p2(100,0);
    Point p3(200,0), p4(500,500);
    int thickness = 2;

    for (;;) { //while loop
        Mat frame;
        cap >> frame;

        //draw lines across frame
        line(frame, p1, p4, Scalar(255, 0, 0), thickness, LINE_8);
        line(frame, p2, p4, Scalar(0, 255, 0), thickness, LINE_4);
        line(frame, p3, p4, Scalar(0, 0, 255), thickness, LINE_AA);

        if (frame.empty()) break; //end of video stream
        imshow("video in", frame);
        if (waitKey(10) == 27) break; //stop by ESC key
    }

    return 0;
}
