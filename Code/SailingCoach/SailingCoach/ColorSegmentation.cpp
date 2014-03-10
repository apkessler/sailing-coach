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

static string intToString(int);


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