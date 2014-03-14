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
#define CAM_NUM 1

/****************************** MODULE VARS ***********************************/

/*************************** MODULE PROTOTYPES ********************************/
void showRawFeed(VideoCapture cap);

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
    

    
    printf("Select mode:\n");
    printf("\t[0] Find checkerboard\n");
    printf("\t[1] Raw Webcam Feed\n");
    printf("\t[2] Track Targets\n");
    printf(">>");
    string usr_in;
    cin >> usr_in;
    Size boardSize;
    switch (usr_in[0])
    {
        case '0':
            printf("Running calibration...\n");
            boardSize.height = 6;
            boardSize.width = 9;
            calibrateCameraFromFeed(cap, 5, boardSize, 1.0f);
            break;
            
        case '1':
            showRawFeed(cap);
            break;
            
        case '2':
        default:
            printf("Running segmentation...\n");
            runColorSegmentation(cap,dWidth,dHeight);
            break;
    }
    
    cap.release();
    
   
    return 0;
}

void showRawFeed(VideoCapture cap)
{
    string rawFeed = "Raw Feed";
    
        // Display the image.
        namedWindow(rawFeed,CV_WINDOW_AUTOSIZE);

        
        Mat thisFrame;
        bool breakLoop = false;
    
        while (!breakLoop)
        {
            bool bSuccess = cap.read(thisFrame); //read new frame
            
            if (!bSuccess)
            {
                printf("Read from video stream failed!\n");
                break;
            }
            
            //resize(thisFrame_rgb, leftFrame, Size(0,0),0.5,0.5);
            imshow(rawFeed, thisFrame);
            
            char user_input = (char) waitKey(30);
            switch (user_input)
            {
                case '\377':
                    //This means nothing was pressed.
                    break;
                case 'f':
                    saveFrameToFile(thisFrame);
                    break;
                    
                case ESC_KEY:
                case 'q':
                    cout << "Quitting!" << endl;
                    breakLoop = true;
                    break;
                    
                default:
                    printf("No behavior defined for '%c'.\n",user_input);
            } //end switch
        } //end while loop
        
        
        destroyWindow(rawFeed);
        thisFrame.release();
}
