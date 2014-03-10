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
#define ESC_KEY 27
#define CAM_NUM 1

//max number of objects to be detected in frame
#define  MAX_NUM_OBJECTS 50
#define  GREEN  Scalar(0,0,255)
#define  MISSED_COUNT_MAX 7
//minimum and maximum object area
double max_object_area, min_object_area;

/****************************** MODULE VARS ***********************************/
//Window names
const string videoFeed = "Video Feed";
const string segFeed = "Segmented Feed";
const string controls = "Controls";

//Control vars
bool trackObjects = true;
bool useMorphOps = true;
bool tuning = false;
bool drawCenter = false;

int missedCount = 0;
bool is_tracking = false;
/*************************** MODULE PROTOTYPES ********************************/
void runColorSegmentation(VideoCapture cap, double dWidth, double dHeight);
void createTrackbars(Threshold_t&, const string);
void morphOps(Mat &);
void drawCenterAxes(Mat&,Size,Scalar);
void drawObject(int, int, Mat &, Scalar);
void trackFilteredObject(Mat threshold, Mat &cameraFeed, Point2d&);
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
    
    max_object_area = dHeight*dWidth*3/2;
    min_object_area = 100;
    
    
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


void runColorSegmentation(VideoCapture cap, double dWidth, double dHeight)
{
    
    // Display the image.
    namedWindow(videoFeed,CV_WINDOW_AUTOSIZE);
    
    //Create settings struct
    Threshold_t color1_limits;
    createTrackbars(color1_limits, controls);
    
    
    Mat thisFrame_rgb;
    Mat thisFrame_hsv;
    Mat thisFrame_seg;
    Mat leftFrame, rightFrame, dispFrame;
    bool breakLoop = false;
    Point2d boomPoint = Point2d(-1,-1);
    
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
        
        if(trackObjects)
        {
            trackFilteredObject(thisFrame_seg, thisFrame_rgb, boomPoint);
        }
        
        
        cvtColor(thisFrame_seg, thisFrame_seg,COLOR_GRAY2BGR); //convert back to RGB
        
        
        if (drawCenter)
        {
            drawCenterAxes(thisFrame_rgb, Size(dWidth,dHeight),Scalar(255,255,255));
            drawCenterAxes(thisFrame_seg, Size(dWidth,dHeight),Scalar(0,255,0));
        }
        
        //Resize and plot
        resize(thisFrame_rgb, leftFrame, Size(0,0),0.5,0.5);
        resize(thisFrame_seg, rightFrame, Size(0,0),0.5,0.5);
        hconcat(leftFrame, rightFrame, dispFrame);
        imshow(videoFeed, dispFrame);
   
        
        char user_input = (char) waitKey(30);
        string colorStr;
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
                printf("Enter color to write >> ");
                cin >> colorStr;
                saveSettingsToFile(color1_limits, colorStr);
                break;
            case 'r':
                printf("Enter color to load >> ");
                cin >> colorStr;
                readSettingsFromFile(color1_limits, colorStr);
                printf("Exiting read in from file...\r\n");
                break;
            case 'c':
                drawCenter = !drawCenter;
                printf("Draw center axes %s.\n", drawCenter ? "ON" : "OFF");
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
    thisFrame_rgb.release();
    thisFrame_hsv.release();
    thisFrame_seg.release();

}


void drawCenterAxes(Mat &frame, Size theSize,Scalar theColor)
{
    int x_center = theSize.width/2;
    int y_center = theSize.height/2;
    line(frame,Point(0,y_center),Point(theSize.width,y_center),theColor,1);
    line(frame,Point(x_center, 0), Point(x_center, theSize.height),theColor,1);
    circle(frame,Point(x_center,y_center),20,theColor,1);
    
}

/*
* This function looks for objects in the thresholded image.
*/
void trackFilteredObject(Mat threshold, Mat &cameraFeed, Point2d &dst){
    
	Mat temp;
	threshold.copyTo(temp);
    
	int x = 0, y = 0;

    
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
    
	//find contours of filtered image using openCV findContours function
	findContours(temp,contours,hierarchy,CV_RETR_CCOMP,CV_CHAIN_APPROX_SIMPLE );
    
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0)
	{
		int numObjects = hierarchy.size();
        
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
        if(numObjects<MAX_NUM_OBJECTS)
        {
			for (int index = 0; index >= 0; index = hierarchy[index][0])
			{
                
				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;
                
				//if the area is less than 400px^2 then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
                if(area>min_object_area && area<max_object_area && area>refArea)
				{
					x = moment.m10/area;
					y = moment.m01/area;
					objectFound = true;
                    
				}
				else
				{
					objectFound = false;
				}
                
			}
			//let user know you found an object
			if(objectFound == true)
			{
				putText(cameraFeed,"Tracking Object",Point(0,50),2,1,Scalar(0,255,0),2);
                
				//draw object location on screen
				drawObject(x,y, cameraFeed, GREEN);
			}
		}
        else
        {
			putText(cameraFeed,"TOO MUCH NOISE! ADJUST FILTER",Point(0,50),1,2,Scalar(0,0,255),2);
			objectFound = false;
        }
	}
    
    
	if (objectFound)
	{
		missedCount = 0;
		is_tracking = true;
        dst.x = x;
        dst.y = y;
        
	}
	else //object not found
	{
		if (is_tracking) //if it was tracking an object
		{
			if ((missedCount) < MISSED_COUNT_MAX)
			{
				//don't change
				missedCount++;
			}
			else //to many missed tracks
			{

				is_tracking = false;
                dst.x = -1;
                dst.y = -1; //indicate a reset
			}
            
		}
		else
		{
			dst.x = -1;
            dst.y = -1;
		}
	}
    
}

/*
 * This function draws the crosshair symbol over the tracked object, along with the pixel
 * coordinates as text.
 */
void drawObject(int x, int y, Mat &frame, Scalar clr){
    
	//use some of the openCV drawing functions to draw crosshairs
	//on your tracked image!
    
    
	circle(frame,Point(x,y),20,clr,2);
	line(frame,Point(x,y-5),Point(x,y-25),clr,2);
	line(frame,Point(x,y+5),Point(x,y+25),clr,2);
	line(frame,Point(x-5,y),Point(x-25,y),clr,2);
	line(frame,Point(x+5,y),Point(x+25,y),clr,2);
    
    char buffer[16];
    sprintf(buffer,"(%d,%d)",x,y);
	putText(frame,buffer,Point(x,y+30),1,1,clr,2);
    
}
