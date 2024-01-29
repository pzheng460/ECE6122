/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Sep 17, 2023

Description:

It's a class to store and initialize the state of each point charge.

*/

#include "ECE_PointCharge.h"

// Constructor implementation
ECE_PointCharge::ECE_PointCharge(double x, double y, double z, double q)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->q = q;
}

// set the location of point charge
void ECE_PointCharge::setLocation(double x, double y, double z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

// set charge of point charge
void ECE_PointCharge::setCharge(double q)
{
    this->q = q;
}