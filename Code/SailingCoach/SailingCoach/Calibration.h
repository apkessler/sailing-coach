//
//  Calibration.h
//  SailingCoach
//
//  Created by Andrew Kessler on 3/10/14.
//  Copyright (c) 2014 Andrew Kessler. All rights reserved.
//

#ifndef __SailingCoach__Calibration__
#define __SailingCoach__Calibration__

#include <iostream>
#include "opencv2/opencv.hpp"
using namespace cv;

int calibrateCameraFromFeed(VideoCapture capture, int nframes, Size boardSize, float squareSize);
#endif /* defined(__SailingCoach__Calibration__) */
