/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Sep 17, 2023

Description:

It's a header file for ECE_PointCharge class to declare the member variables and functions of the class.

*/

#ifndef LAB1_ECE_POINTCHARGE_H
#define LAB1_ECE_POINTCHARGE_H


class ECE_PointCharge {
protected:
    double x; // x-coordinate.
    double y; // y-coordinate.
    double z; // z-coordinate.
    double q; // charge of the point.

public:
    ECE_PointCharge(double x, double y, double z, double q);
    void setLocation(double x, double y, double z);
    void setCharge(double q);
};


#endif //LAB1_ECE_POINTCHARGE_H
