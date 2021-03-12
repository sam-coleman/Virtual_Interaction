#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdlib.h>
#include <tuple>

using namespace cv;
using namespace std;

int main(int argc, char** argv ) {

    VideoCapture cap;
    if (!cap.open(-1)) {
    //if(!cap.open(-1)) {
    return 0;
    //}
    }

    //DEQUEUE FUNCTION
    deque<int> bpoints;
    bpoints.max_size();
    deque<int> gpoints;
    gpoints.max_size();
    deque<int> rpoints;
    rpoints.max_size();
    deque<int> ypoints;
    ypoints.max_size();
    int bindex=0, gindex=0, rindex=0, yindex=0, colorIndex = 0; 
    //Colors are BGR
    tuple <int, int, int> blue;
    tuple <int, int, int> green;
    tuple <int, int, int> red;
    tuple <int, int, int> yellow;
    blue = make_tuple(255, 0, 0);
    //can we make an array of tuples??

    Mat Window;
    Mat video;
    cap >> video;
    if(Window.empty()) Window = cv::Mat::zeros(video.size(), video.type());

    //CREATE RECTANGLES FOR COLOR OPTIONS
    //TODO: get Scalar value from color (replace Sclar(#,#,#)) with tuples defined above
    rectangle(Window, Point(40,1), Point(140,65), Scalar(255,255,255), -1);
    rectangle(Window, Point(160,1), Point(255,65), Scalar(255,0,0), -1);
    rectangle(Window, Point(275,1), Point(370,65), Scalar(0,255,0), -1);
    rectangle(Window, Point(390,1), Point(485,65), Scalar(0,0,255), -1);
    rectangle(Window, Point(505,1), Point(600,65), Scalar(0,255,255), -1);
    putText(Window, "CLEAR ALL", Point(49, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(0,0,0), 1, LINE_AA);
    putText(Window, "BLUE", Point(185, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1, LINE_AA);
    putText(Window, "GREEN", Point(298, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1, LINE_AA);
    putText(Window, "RED", Point(420, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(255,255,255), 1, LINE_AA);
    putText(Window, "YELLOW", Point(520, 33), FONT_HERSHEY_SIMPLEX, .5, Scalar(150,150,150), 1, LINE_AA);
    double alpha = .5, gamma = 0;
   
    for(;;) {
        cap >> video;
        flip(video, video, +1);
        Mat combined;
        addWeighted(Window, alpha, video, 1-alpha, gamma, combined);

        if (Window.empty()) break; //end of video stream
        imshow("video in", combined);
        if (waitKey(10) == 27) break; //stop by ESC key
    }
    return 0;
}