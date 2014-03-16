
//
//  BoatModel.cpp
//  SailingCoach
//
//  Created by Andrew Kessler on 3/14/14.
//  Copyright (c) 2014 Andrew Kessler. All rights reserved.
//

#include "BoatModel.h"
#include "opencv2/opencv.hpp"
using namespace cv;
using namespace std;


Mat A;


float calculateBoomAngle(Point2d boomFoot_im)
{
    
    //Calculate the angle!
    
    
    return 0.0;
}


int readCameraMatrixFromFile(string fileName)
{
    
    printf("Reading camera matrix...\r\n");
    
	FileStorage fs(fileName + "_data.yml",FileStorage::READ);
    
    
	if (!fs.isOpened())
	{
		cout << "No file " << fileName << "_data.yml" << " was found." << endl;
		return 0;
	}
	cout << "opened file " << fileName <<"_data.yml...";
    
	fs["camera_matrix"] >> A;

	fs.release();
    
	printf("done!\r\n");
    
    cout << "A = " << endl << " " << A << endl << endl;
    
    return 1;
    
}