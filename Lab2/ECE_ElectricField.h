/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Sep 17, 2023

Description:

It's a header file for ECE_ElectricField class to declare the member variables and functions of the class.

*/

#ifndef LAB1_ECE_ELECTRICFIELD_H
#define LAB1_ECE_ELECTRICFIELD_H

#include "ECE_PointCharge.h"

class ECE_ElectricField : public ECE_PointCharge {
protected:
    double Ex; // Electric field in the x-direction.
    double Ey; // Electric field in the y-direction.
    double Ez; // Electric field in the z-direction.

public:
    ECE_ElectricField(double x, double y, double z, double q);
    void computeFieldAt(double x, double y, double z);
    void getElectricField(double &Ex, double &Ey, double &Ez);
};


#endif //LAB1_ECE_ELECTRICFIELD_H
