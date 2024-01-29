/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Nov 30, 2023

Description:

Use OpenMPI to estimate the value of a definite integral using the Monte Carlo method.

*/
#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <cmath>
#include <ctime>

// Function to calculate the value of the integral
double MonteCarloIntegral(int samples)
{
    double sum = 0.0;
    for (int i = 0; i < samples; ++i) {
        double x = static_cast<double>(rand()) / RAND_MAX; // Uniformly distributed in [0, 1]
        sum += x * x;
    }
    return sum;
}

// Function to calculate the value of the second integral
double MonteCarloIntegralExp(int samples)
{
    double sum = 0.0;
    for (int i = 0; i < samples; ++i)
    {
        double x = static_cast<double>(rand()) / RAND_MAX; // Uniformly distributed in [0, 1]
        sum += exp(-x * x);  // Calculate e^(-x^2)
    }
    return sum;
}

int main(int argc, char *argv[])
{
    int rank, size, N, P, seed;
    double integral, total;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 5)
    {
        if (rank == 0)
        {
            std::cout << "Usage: " << argv[0] << " -P <1 or 2> -N <number of samples>" << std::endl;
        }
        MPI_Finalize();
        return 1;
    }

    // Parsing command line arguments
    P = atoi(argv[2]);
    N = atoi(argv[4]);

    int sampleSize = N / size;

    // Broadcasting the seed to all processes
    MPI_Bcast(&seed, 1, MPI_INT, 0, MPI_COMM_WORLD);

    srand(seed + rank);  // Each process now has a different random seed

    if (P == 1)
    {
        integral = MonteCarloIntegral(sampleSize);
    }
    else if (P == 2)
    {
        integral = MonteCarloIntegralExp(sampleSize);
    }

    // Gathering results from all processes
    MPI_Reduce(&integral, &total, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        std::cout << "The estimate for integral " << P << " is " << total / N << std::endl;
        std::cout << "Bye!" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
