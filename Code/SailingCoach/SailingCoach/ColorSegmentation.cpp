//
//  SettingsLibrary.cpp
//  SailingCoach
//
//  Created by Andrew Kessler on 3/7/14.
//  Copyright (c) 2014 Andrew Kessler. All rights reserved.
//

#include "ColorSegmentation.h"
#include "opencv2/opencv.hpp"

using namespace std;
using namespace cv;
/****************************** MODULE DEFINES*********************************/
#define CAM_NUM 1
#define MISSED_COUNT_MAX 7
#define MAX_NUM_OBJECTS 50
#define ESC_KEY 27

#define PI 3.14159265
/****************************** MODULE VARS ***********************************/
static string intToString(int);
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


//minimum and maximum object area
double max_object_area, min_object_area;

/*************************** MODULE PROTOTYPES ********************************/


void saveFrameToFile(Mat &theFrame);



/**************************** MODULE BODIES ***********************************/

void onMouse(int event, int x, int y, int, void*)
{
    if (event == CV_EVENT_LBUTTONDOWN)
    {
        printf("Mouse @ (%d,%d)\n",x,y);
    }
}



/*
 * Takes the current trackbar settings and writes them to a file.
 */
void saveSettingsToFile(Threshold_t &colorSettings, string clrStr)
{
	printf("Saving HSV settings...");
    
	FileStorage fs(clrStr + "_data.xml",FileStorage::WRITE);
    
	if (!fs.isOpened())
	{
		cout << "Error creating file " << clrStr << "_data.xml" << "." << endl;
		return;
	}
    
	fs << "H_MIN" << colorSettings.h_min;
	fs << "H_MAX" << colorSettings.h_max;
    
	fs << "S_MIN" << colorSettings.s_min;
	fs << "S_MAX" << colorSettings.s_max;
    
	fs << "V_MIN" << colorSettings.v_min;
	fs << "V_MAX" << colorSettings.v_max;
    
	fs.release();
	printf("done!\r\n");
}

/*
 * Loads the hsv_data.xml file and writes settings to the trackbars.
 */
void readSettingsFromFile(Threshold_t &colorSettings, string clrStr)
{
	printf("Reading HSV settings...\r\n");
    
	FileStorage fs(clrStr + "_data.xml",FileStorage::READ);
    
	string name = "Color " + intToString(colorSettings.ID) + " Settings";
    
	if (!fs.isOpened())
	{
		cout << "No file " << clrStr << "_data.xml" << " was found." << endl;
		return;
	}
	cout << "opened file " << clrStr <<"_data.xml...";
    
	fs["H_MIN"] >> colorSettings.h_min;
	fs["H_MAX"] >> colorSettings.h_max;
    
	fs["S_MIN"] >> colorSettings.s_min;
	fs["S_MAX"] >> colorSettings.s_max;
    
	fs["V_MIN"] >> colorSettings.v_min;
	fs["V_MAX"] >> colorSettings.v_max;
    
	setTrackbarPos("H_MIN",name,colorSettings.h_min);
	setTrackbarPos("H_MAX",name,colorSettings.h_max);
	setTrackbarPos("S_MIN",name,colorSettings.s_min);
	setTrackbarPos("S_MAX",name,colorSettings.s_max);
	setTrackbarPos("V_MIN",name,colorSettings.v_min);
	setTrackbarPos("V_MAX",name,colorSettings.v_max);
    
	fs.release();
    
	printf("done!\r\n");
    
    
}


/*
 * This function creates the window that has the trackbar that control the HSV
 * thresholds.
 * For some reason when you try to add the trackbars to the same window, the order in which they appear
 * is totally random and unchangable. So, we just make 3 windows each with only two sliders.
 */
void createTrackbars(Threshold_t& settings, const string name){
	//create window for trackbars
    
    namedWindow(name,0);

	//create trackbars and insert them into window
    createTrackbar( "V_MIN", name, &(settings.v_min), 256 );//e
    createTrackbar( "S_MIN", name, &(settings.s_min), 256 );//c
    createTrackbar( "H_MIN", name, &(settings.h_min), 256 );//a
    createTrackbar( "V_MAX", name, &(settings.v_max), 256 );//f
    createTrackbar( "S_MAX", name, &(settings.s_max), 256 );//d
    createTrackbar( "H_MAX", name, &(settings.h_max), 256 );//b
    
    
}


static string intToString(int number)
{
	std::stringstream ss;
	ss << number;
	return ss.str();
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

void runColorSegmentation(VideoCapture cap, double dWidth, double dHeight)
{
    
    // Display the image.
    namedWindow(videoFeed,CV_WINDOW_AUTOSIZE);
    
    //Create settings struct
    Threshold_t color1_limits;
    createTrackbars(color1_limits, controls);
    
    max_object_area = dHeight*dWidth*3/2;
    min_object_area = 100;
    
    Mat thisFrame_rgb;
    Mat thisFrame_rgb_orig;
    Mat thisFrame_hsv;
    Mat thisFrame_seg;
    Mat leftFrame, rightFrame, dispFrame;
    bool breakLoop = false;
    Point2d boomPoint = Point2d(-1,-1);
    setMouseCallback(videoFeed, onMouse);
    while (!breakLoop)
    {
        bool bSuccess = cap.read(thisFrame_rgb_orig); //read new frame
        thisFrame_rgb_orig.copyTo(thisFrame_rgb);
        
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
        
        if (boomPoint.x >= 0 && boomPoint.y >=0 )
        {
            line(thisFrame_rgb, Point(dWidth/2,dHeight/2), boomPoint, BLUE,3);
            line(thisFrame_seg, Point(dWidth/2,dHeight/2), boomPoint, BLUE,3);
            
            char buffer[16];
            int dx = boomPoint.x - dWidth/2;
            sprintf(buffer,"dx = %d px",dx);
            putText(thisFrame_rgb,buffer,Point(dWidth - 200,80),1.5,1.5,BLUE,2);
           
            sprintf(buffer,"Angle = %2.1f deg",asin(dx/BOOM_LENGTH)*180.0/PI);
            putText(thisFrame_rgb,buffer,Point(dWidth/3,dHeight-100),2,2,BLUE,2);
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
            case 'f':
                saveFrameToFile(thisFrame_rgb_orig);
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
 * This function applies morphological operations to the thresholded image.
 * It runs two erodes to reduce noise, and two dilates to enlarge remaining
 * white space.
 */
void morphOps(Mat &thresh)
{
    
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

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    
    return buf;
}




void saveFrameToFile(Mat &theFrame)
{
    string fileStr;
    printf("Saving file...\n");
    fileStr = "frame_" + currentDateTime() + ".jpg";
    imwrite(fileStr, theFrame);
    cout << "Wrote " << fileStr << endl;
}


