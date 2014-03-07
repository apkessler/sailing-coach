//
//  main.cpp
//  SailingCoach
//
//  Created by Andrew Kessler on 3/7/14.
//  Copyright (c) 2014 Andrew Kessler. All rights reserved.
//

#include <iostream>
#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;


int main(int argc, const char * argv[])
{
    // Open the file.
    
    printf("Starting CV SailingCoach!\n");
    
    printf("Attempting to open camera feed...");
    VideoCapture cap(0); //open the first camera on list
    
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
    
    IplImage *img = cvCreateImage( cvSize(100,200), IPL_DEPTH_8U, 3);
    
    if (!img)
    {
        printf("Error: Couldn't open the image file.\n");
        return 1;
    }
    
    // Display the image.
    cvNamedWindow("VideoFeed", CV_WINDOW_AUTOSIZE);
    
    Mat thisFrame;
    while (1)
    {
        bool bSuccess = cap.read(thisFrame); //rad new frame
        
        if (!bSuccess)
        {
            printf("Read from video stream failed!\n");
            break;
        }
        
        imshow("VideoFeed", thisFrame);
        
        if (waitKey(30) == 27) //wait 30ms for ESC key
        {
            printf("User exit command received.\n");
            break;
        }
        
    }
    destroyWindow("VideoFeed");
    cap.release();
    thisFrame.release();
    
    return 0;
}

