#include <stdio.h>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp> //blur efekti için
#include <opencv2/video/background_segm.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

//DEFINE GLOBAL VARIABLES  

// hand_hist = None
// traverse_point = []
int total_rectangle = 9;
//if mad, switch to uint32
vector <int> hand_rect_one_x;
vector <int> hand_rect_one_y;

vector <int> hand_rect_two_x;
vector <int> hand_rect_two_y;

//THIS IS USED OTHER PLACES BUT DOESNT NEED TO BE
// Mat rescale_frame(Mat frame, int wpercent=130, int hpercent=130){
//     int width = int(frame.size[1] * wpercent / 100);
//     int height = int(frame.size[0] * hpercent / 100);
//     resizeWindow(frame, width, height);
//     return frame;
// }


vector<vector<Point>> contours(Mat hist_mask_mat){
    Mat gray_hist_mask_mat;
    Mat threshold_mat;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    cvtColor(hist_mask_mat, gray_hist_mask_mat, COLOR_BGR2GRAY);
    threshold( gray_hist_mask_mat, threshold_mat, 0, 255, 0);
    findContours(threshold_mat, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );
    return contours;
}

Mat draw_rect(Mat frame){
    Size sz = frame.size();
    int rows = sz.height;
    int cols = sz.width;

    //global total_rectangle, hand_rect_one_x, hand_rect_one_y, hand_rect_two_x, hand_rect_two_y

    hand_rect_one_x.insert(hand_rect_one_x.end(), {6 * rows / 20, 6 * rows / 20, 6 * rows / 20, 9 * rows / 20, 9 * rows / 20, 9 * rows / 20, 12 * rows / 20,
         12 * rows / 20, 12 * rows / 20});

    hand_rect_one_y.insert(hand_rect_one_y.end(), {9 * cols / 20, 10 * cols / 20, 11 * cols / 20, 9 * cols / 20, 10 * cols / 20, 11 * cols / 20, 9 * cols / 20,
         10 * cols / 20, 11 * cols / 20});

    hand_rect_two_x.insert(hand_rect_two_x.end(), {6 * rows / 20 + 10, 6 * rows / 20 + 10, 6 * rows / 20 + 10, 9 * rows / 20 + 10, 9 * rows / 20 + 10, 9 * rows / 20 + 10, 12 * rows / 20 + 10,
         12 * rows / 20 + 10, 12 * rows / 20 + 10});

    hand_rect_two_y.insert(hand_rect_two_y.end(), {9 * cols / 20 + 10, 10 * cols / 20 + 10, 11 * cols / 20 + 10, 9 * cols / 20 + 10, 10 * cols / 20 + 10, 11 * cols / 20 + 10, 9 * cols / 20 + 10,
         10 * cols / 20 + 10, 11 * cols / 20 + 10});

    // hand_rect_two_x = hand_rect_one_x + 10;
    // hand_rect_two_y = hand_rect_one_y + 10;

    for (int i = 0; i < total_rectangle; i++) {
        rectangle(frame, Point(hand_rect_one_y.at(i), hand_rect_one_y.at(i)), Point(hand_rect_two_y[i], hand_rect_two_x[i]), (0,255,0),1);
    }

    return frame;
}

//https://docs.opencv.org/3.4/d8/dbc/tutorial_histogram_calculation.html
Mat hand_histogram(Mat frame){
    Mat hand_hsv;
    int histSize = 256;
    float range[] = { 0, 256 }; //the upper boundary is exclusive
    const float* histRange = { range };
    vector<Mat> hsv_planes;
    split(hand_hsv, hsv_planes );
    bool uniform = true, accumulate = false;
    Mat h_hist, s_hist, v_hist;
    cvtColor(frame, hand_hsv, COLOR_BGR2HSV);
    //this is 90 by ten because there are 9, 10x10 rectangles combined
    Rect roi(0, 0, 90, 10); 

    calcHist( &hsv_planes[0], 1, 0, Mat(), h_hist, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &hsv_planes[1], 1, 0, Mat(), s_hist, 1, &histSize, &histRange, uniform, accumulate );
    calcHist( &hsv_planes[2], 1, 0, Mat(), v_hist, 1, &histSize, &histRange, uniform, accumulate );

    return cv2.normalize(hand_hist, hand_hist, 0, 255, cv2.NORM_MINMAX)
}


def hist_masking(frame, hist):
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    dst = cv2.calcBackProject([hsv], [0, 1], hist, [0, 180, 0, 256], 1)

    disc = cv2.getStructuringElement(cv2.MORPH_ELLIPSE, (31, 31))
    cv2.filter2D(dst, -1, disc, dst)

    ret, thresh = cv2.threshold(dst, 150, 255, cv2.THRESH_BINARY)

    # thresh = cv2.dilate(thresh, None, iterations=5)

    thresh = cv2.merge((thresh, thresh, thresh))

    return cv2.bitwise_and(frame, thresh)


def centroid(max_contour):
    moment = cv2.moments(max_contour)
    if moment['m00'] != 0:
        cx = int(moment['m10'] / moment['m00'])
        cy = int(moment['m01'] / moment['m00'])
        return cx, cy
    else:
        return None


def farthest_point(defects, contour, centroid):
    if defects is not None and centroid is not None:
        s = defects[:, 0][:, 0]
        cx, cy = centroid

        x = np.array(contour[s][:, 0][:, 0], dtype=np.float)
        y = np.array(contour[s][:, 0][:, 1], dtype=np.float)

        xp = cv2.pow(cv2.subtract(x, cx), 2)
        yp = cv2.pow(cv2.subtract(y, cy), 2)
        dist = cv2.sqrt(cv2.add(xp, yp))

        dist_max_i = np.argmax(dist)

        if dist_max_i < len(s):
            farthest_defect = s[dist_max_i]
            farthest_point = tuple(contour[farthest_defect][0])
            return farthest_point
        else:
            return None


def draw_circles(frame, traverse_point):
    if traverse_point is not None:
        for i in range(len(traverse_point)):
            cv2.circle(frame, traverse_point[i], int(5 - (5 * i * 3) / 100), [0, 255, 255], -1)


def manage_image_opr(frame, hand_hist):
    hist_mask_image = hist_masking(frame, hand_hist)

    hist_mask_image = cv2.erode(hist_mask_image, None, iterations=2)
    hist_mask_image = cv2.dilate(hist_mask_image, None, iterations=2)

    contour_list = contours(hist_mask_image)
    max_cont = max(contour_list, key=cv2.contourArea)

    cnt_centroid = centroid(max_cont)
    cv2.circle(frame, cnt_centroid, 5, [255, 0, 255], -1)

    if max_cont is not None:
        hull = cv2.convexHull(max_cont, returnPoints=False)
        defects = cv2.convexityDefects(max_cont, hull)
        far_point = farthest_point(defects, max_cont, cnt_centroid)
        print("Centroid : " + str(cnt_centroid) + ", farthest Point : " + str(far_point))
        cv2.circle(frame, far_point, 5, [0, 0, 255], -1)
        if len(traverse_point) < 20:
            traverse_point.append(far_point)
        else:
            traverse_point.pop(0)
            traverse_point.append(far_point)

        draw_circles(frame, traverse_point)


int main(){
    global hand_hist
    is_hand_hist_created = False
    capture = cv2.VideoCapture(0)

    while capture.isOpened():
        pressed_key = cv2.waitKey(1)
        _, frame = capture.read()
        frame = cv2.flip(frame, 1)

        if pressed_key & 0xFF == ord('z'):
            is_hand_hist_created = True
            hand_hist = hand_histogram(frame)

        if is_hand_hist_created:
            manage_image_opr(frame, hand_hist)

        else:
            frame = draw_rect(frame)

        cv2.imshow("Live Feed", rescale_frame(frame))

        if pressed_key == 27:
            break

    cv2.destroyAllWindows()
    capture.release()
}