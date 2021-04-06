#pragma once

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <stdlib.h>
#include <tuple>

cv::Scalar GetColorFromTuple(std::tuple<int, int, int>);

void UpdateThickBoxes(cv::Mat, cv::Rect, cv::Rect, cv::Rect, cv::Rect, int);

void InitControls(cv::Mat, cv::Rect, cv::Rect, cv::Rect, cv::Rect, cv::Rect, 
                  cv::Rect, cv::Rect, std::vector<std::tuple<int, int, int>>);

