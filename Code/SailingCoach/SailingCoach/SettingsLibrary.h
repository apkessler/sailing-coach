//
//  SettingsLibrary.h
//  SailingCoach
//
//  Created by Andrew Kessler on 3/7/14.
//  Copyright (c) 2014 Andrew Kessler. All rights reserved.
//

#ifndef __SailingCoach__SettingsLibrary__
#define __SailingCoach__SettingsLibrary__

#include <iostream>

//This is a struct type that will hold min/max in HSV space
typedef struct
{
    int h_min = 0;
    int h_max = 255;
    int s_min = 0;
    int s_max = 255;
    int v_min = 0;
    int v_max = 255;
    int ID;
} Threshold_t;

void readSettingsFromFile(Threshold_t &, std::string);
void saveSettingsToFile(Threshold_t &, std::string);
void createTrackbars(Threshold_t&, const std::string);


#endif /* defined(__SailingCoach__SettingsLibrary__) */
