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

using namespace cv;
using namespace std;

#define ESC_KEY 27



/******************* GLOBAL VARS **********************/

//Window names
const string videoFeed = "Video Feed";
const string segFeed = "Segmented Feed";
const string controls = "Controls";

//Control vars
bool trackObjects = true;
bool useMorphOps = true;
bool tuning = false;

/************* FUNCTION PROTOTYPES ******************/
void createTrackbars(Threshold_t&, const string);
void morphOps(Mat &);

int main(int argc, const char * argv[])
{

    printf("Starting CV SailingCoach!\n");
    printf("Attempting to open camera feed...");
 
    VideoCapture cap(1); //open the first camera on list
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
    
    // Display the image.
    namedWindow(videoFeed,CV_WINDOW_AUTOSIZE);
    namedWindow(segFeed,CV_WINDOW_AUTOSIZE);
    
    //Create settings struct
    Threshold_t color1_limits;
    createTrackbars(color1_limits, controls);
    

    Mat thisFrame_rgb;
    Mat thisFrame_hsv;
    Mat thisFrame_seg;
    bool breakLoop = false;
    
    while (!breakLoop)
    {
        bool bSuccess = cap.read(thisFrame_rgb); //read new frame
        
        if (!bSuccess)
        {
            printf("Read from video stream failed!\n");
            break;
        }
        
        cvtColor(thisFrame_rgb, thisFrame_hsv, COLOR_BGR2HSV); //convert to HSV
        
        //Now we look for the particular color...
        inRange(thisFrame_hsv,
                Scalar(color1_limits.h_min,color1_limits.s_min,color1_limits.v_min),
                Scalar(color1_limits.h_max,color1_limits.s_max,color1_limits.v_max),
                thisFrame_seg);
        
        //Perform morphological operations on thresholded image to eliminate noise
        if (useMorphOps)
        {
            morphOps(thisFrame_seg);
        }
        
        imshow(videoFeed, thisFrame_rgb);
        imshow(segFeed, thisFrame_seg);
        
        char user_input = (char) waitKey(30);
        
        switch (user_input)
		{
            case '\377':
                //This means nothing was pressed.
                break;
                
            case 'm':
                useMorphOps = !useMorphOps;
                printf("Morphological operations %s.\n", useMorphOps ? "ON" : "OFF");
                break;
            case 't':
                trackObjects = !trackObjects;
                printf("Tracking objects %s.\n", trackObjects ? "ON" : "OFF");

                break;
            case 'w':
                tuning = !tuning;
                printf("Tuning %s.\n", tuning ? "ON" : "OFF");
                break;
            case 's':
                saveSettingsToFile(color1_limits, "color1");
                break;
            case 'r':
                readSettingsFromFile(color1_limits, "color1");
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
    
    
    destroyWindow(videoFeed);
    cap.release();
    
    thisFrame_rgb.release();
    thisFrame_hsv.release();
    thisFrame_seg.release();
    
    return 0;
}



/*
 * This function applies morphological operations to the thresholded image.
 * It runs two erodes to reduce noise, and two dilates to enlarge remaining
 * white space.
 */
void morphOps(Mat &thresh){
    
	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle
    
	Mat erodeElement = getStructuringElement( MORPH_RECT,Size(3,3));
    //dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement( MORPH_RECT,Size(8,8));
    
	erode(thresh,thresh,erodeElement);
	erode(thresh,thresh,erodeElement);
    
    
	dilate(thresh,thresh,dilateElement);
	dilate(thresh,thresh,dilateElement);
    
    
    
}



