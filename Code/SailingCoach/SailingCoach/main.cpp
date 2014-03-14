//
//  main.cpp
//  SailingCoach
//
//  Created by Andrew Kessler on 3/7/14.
//  Copyright (c) 2014 Andrew Kessler. All rights reserved.
//

#include <iostream>
#include "opencv2/opencv.hpp"
#include "ColorSegmentation.h"
#include "Calibration.h"

using namespace cv;
using namespace std;


/****************************** MODULE DEFINES*********************************/
#define CAM_NUM 0

/****************************** MODULE VARS ***********************************/

/*************************** MODULE PROTOTYPES ********************************/


/**************************** MODULE BODIES ***********************************/
int main(int argc, const char * argv[])
{

    printf("Starting CV SailingCoach!\n");
    printf("Attempting to open camera feed...");
 
    VideoCapture cap(CAM_NUM); //open the first camera on list
    if (!cap.isOpened())
    {
        printf("Couldn't open the camera!\n");
        return 1;
    }
    else
    {
        printf("done!\n");
    }
    
    
    double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of video frames
    double dHeight= cap.get(CV_CAP_PROP_FRAME_HEIGHT);//get the height of video frames
    printf("Frame size is %dx%d.\n", (int) dWidth,(int) dHeight);
    

    
    printf("Calibrate camera? [Y/n] ==>");
    string usr_in;
    cin >> usr_in;

    if (usr_in[0] == 'Y')
    {
        printf("Running calibration...\n");
        Size boardSize;
        boardSize.height = 6;
        boardSize.width = 9;
        
        calibrateCameraFromFeed(cap, 5, boardSize, 1.0f);
    }
    else
    {
        printf("Running segmentation...\n");
        runColorSegmentation(cap,dWidth,dHeight);
    }
    
    cap.release();
    
   
    return 0;
}





