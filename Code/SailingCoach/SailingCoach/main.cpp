//
//  main.cpp
//  SailingCoach
//
//  Created by Andrew Kessler on 3/7/14.
//  Copyright (c) 2014 Andrew Kessler. All rights reserved.
//

#include <iostream>
#include "opencv2/opencv.hpp"

int main(int argc, const char * argv[])
{
    // Open the file.
    
    IplImage *img = cvCreateImage( cvSize(100,200), IPL_DEPTH_8U, 3);
    
    if (!img)
    {
        printf("Error: Couldn't open the image file.\n");
        return 1;
    }
    
    // Display the image.
    cvNamedWindow("Image:", CV_WINDOW_AUTOSIZE);
    cvShowImage("Image:", img);
    
    // Wait for the user to press a key in the GUI window.
    cvWaitKey(0);
    // Free the resources.
    cvDestroyWindow("Image:");
    cvReleaseImage(&img);
    
    return 0;
}

