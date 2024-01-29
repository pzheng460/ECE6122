/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Sep 17, 2023

Description:

It's a class to compute and get the electric field of a point charge at a specific point (x, y, z), which inherits the class of ECE_PointCharge to initialize the point charge.

*/

#include "ECE_ElectricField.h"
#include <cmath>

const double k = 9.0e9; // Coulomb's constant

// Constructor implementation
ECE_ElectricField::ECE_ElectricField(double x, double y, double z, double q) : ECE_PointCharge(x, y, z, q)
{
    this->x = x;
    this->y = y;
    this->z = z;
    this->q = q;
}

void ECE_ElectricField::computeFieldAt(double x, double y, double z)
{
    // Calculate electric field using the formula
    double dx = x - this->x;
    double dy = y - this->y;
    double dz = z - this->z;
    double r = sqrt(dx * dx + dy * dy + dz * dz);

    if (r == 0.0) {
        // Avoid division by zero
        Ex = Ey = Ez = 0.0;
    } else {
        double E = k * q / (r * r);
        Ex = E * (dx / r);
        Ey = E * (dy / r);
        Ez = E * (dz / r);
    }
}

void ECE_ElectricField::getElectricField(double &Ex, double &Ey, double &Ez)
{
    Ex = this->Ex;
    Ey = this->Ey;
    Ez = this->Ez;
}