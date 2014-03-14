//
//  SettingsLibrary.h
//  SailingCoach
//
//  Created by Andrew Kessler on 3/7/14.
//  Copyright (c) 2014 Andrew Kessler. All rights reserved.
//

#ifndef __SailingCoach__ColorSegmentation__
#define __SailingCoach__ColorSegmentation__

#include <iostream>
#include "opencv2/opencv.hpp"
using namespace cv;

//This is a struct type that will hold min/max in HSV space
typedef struct
{
    int h_min = 0;
    int h_max = 255;
    int s_min = 0;
    int s_max = 255;
    int v_min = 0;
    int v_max = 255;
    int ID;
} Threshold_t;

void readSettingsFromFile(Threshold_t &, std::string);
void saveSettingsToFile(Threshold_t &, std::string);


void drawObject(int, int, Mat &, Scalar);
void trackFilteredObject(Mat threshold, Mat &cameraFeed, Point2d&);
void runColorSegmentation(VideoCapture cap, double dWidth, double dHeight);
void createTrackbars(Threshold_t&, const string);
void morphOps(Mat &);
void drawCenterAxes(Mat&,Size,Scalar);
void saveFrameToFile(Mat &theFrame);

#define  GREEN  Scalar(0,255,0)
#define  BLUE   Scalar(255,0,0)
#define BOOM_LENGTH 494.0
#define ESC_KEY 27
#endif /* defined(__SailingCoach__SettingsLibrary__) */
