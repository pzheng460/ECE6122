/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Nov 4, 2023

Description:

Implement a CUDA program to simulate a 2D random walk. A random walk is a mathematical process that describes a path consisting of a sequence of random steps. In this assignment, you will simulate a large number of walkers taking steps either north, south, east, or west on a grid, and calculate the average distance they travel from the origin.

*/
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <chrono>

// Define aliases for the chrono literals and clock to simplify the code
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;

// Function to perform calculations using cudaMalloc
__global__ void calculateWithCudaMalloc(int64_t numWalkers, int64_t numSteps, float* distances, int** states)
{
    int walkerId = blockIdx.x * blockDim.x + threadIdx.x;
    if (walkerId < numWalkers)
    {
        int x = 0;
        int y = 0;
        for (int i = 0; i < numSteps; i++)
        {
            // Simulate a random step in 2D space
            int direction = states[walkerId][i] % 4;
            if (direction == 0) x++;
            else if (direction == 1) x--;
            else if (direction == 2) y++;
            else y--;
        }      
        distances[walkerId] = sqrt(static_cast<float>(x * x + y * y));
    }
}

// Function to perform calculations using cudaMallocHost
__global__ void calculateWithCudaMallocHost(int64_t numWalkers, int64_t numSteps, float* distances, int** states)
{
    int walkerId = blockIdx.x * blockDim.x + threadIdx.x;
    if (walkerId < numWalkers)
    {
        int x = 0;
        int y = 0;
        for (int i = 0; i < numSteps; i++)
        {
            // Simulate a random step in 2D space
            int direction = states[walkerId][i] % 4;
            if (direction == 0) x++;
            else if (direction == 1) x--;
            else if (direction == 2) y++;
            else y--;
        }
        distances[walkerId] = sqrt(static_cast<float>(x * x + y * y));
    }
}

// Function to perform calculations using cudaMallocManaged
__global__ void calculateWithCudaManaged(int64_t numWalkers, int64_t numSteps, float* distances, int** states)
{
    int walkerId = blockIdx.x * blockDim.x + threadIdx.x;
    if (walkerId < numWalkers)
    {
        int x = 0;
        int y = 0;
        for (int i = 0; i < numSteps; i++)
        {
            // Simulate a random step in 2D space
            int direction = states[walkerId][i] % 4;
            if (direction == 0) x++;
            else if (direction == 1) x--;
            else if (direction == 2) y++;
            else y--;
        }
        distances[walkerId] = sqrt(static_cast<float>(x * x + y * y));
    }
}

int main(int argc, char* argv[])
{
    int64_t numWalkers = 1000;  // Default value for numWalkers
    int64_t numSteps = 10000;   // Default value for numSteps

    // if (argc != 5) {
    //     std::cerr << "Usage: " << argv[0] << " -W <Number of Walkers> -I <Number of Steps>" << std::endl;
    //     return 1;
    // }

    for (int i = 1; i < argc; i += 2) {
        std::string flag = argv[i];
        if (flag == "-W") numWalkers = std::stoll(argv[i + 1]);  
        else if (flag == "-I") numSteps = std::stoll(argv[i + 1]); 
    }

    if (numWalkers <= 0 || numSteps <= 0) {
        std::cerr << "Invalid input values." << std::endl;
        return 1;
    }

    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));

    int** states;
    cudaMallocHost(&states, numWalkers * sizeof(int*));
    for (int i = 0; i < numWalkers; i++) {
        cudaMallocHost(&states[i], numSteps * sizeof(int));
    }

    // Store the random int
    for (int i = 0; i < numWalkers; i++) {
        for (int j = 0; j < numSteps; j++) {
            states[i][j] = std::rand();  // Generate a random int
        }
    }

    int blockSize = 256;
    int numBlocks = ((numWalkers + blockSize - 1) / blockSize);

    // Start timer for cudaMalloc
    auto startCudaMalloc = high_resolution_clock::now();
    // Allocate memory for distances using different methods
    float* distancesCudaMalloc = nullptr;
    cudaMalloc((void**)&distancesCudaMalloc, numWalkers * sizeof(float));
    calculateWithCudaMalloc<<<numBlocks, blockSize>>>(numWalkers, numSteps, distancesCudaMalloc, states);
    cudaDeviceSynchronize();  // Wait for the GPU to finish
    // Copy data from GPU
    float* distancesCudaMallocCPU = (float*)malloc(numWalkers * sizeof(float));
    cudaMemcpy(distancesCudaMallocCPU, distancesCudaMalloc, numWalkers * sizeof(float), cudaMemcpyDeviceToHost);
    // Calculate and print the average distance from the origin
    float avgDistanceCudaMalloc = 0.0f;

    for (int i = 0; i < numWalkers; i++)
    {
        avgDistanceCudaMalloc += distancesCudaMallocCPU[i];
    }

    avgDistanceCudaMalloc /= static_cast<float>(numWalkers);
    // Clean up
    cudaFree(distancesCudaMalloc);
    free(distancesCudaMallocCPU);
    auto stopCudaMalloc = high_resolution_clock::now();
    // Calculate the elapsed time in milliseconds
    auto microsecondsCudaMalloc = duration_cast<microseconds>(stopCudaMalloc - startCudaMalloc).count();



    // Start timer for cudaMallocHost
    auto startCudaMallocHost = high_resolution_clock::now();
    // Allocate memory for distances using different methods
    float* distancesCudaMallocHost = nullptr;
    cudaMallocHost((void**)&distancesCudaMallocHost, numWalkers * sizeof(float));
    calculateWithCudaMallocHost<<<numBlocks, blockSize>>>(numWalkers, numSteps, distancesCudaMallocHost, states);
    cudaDeviceSynchronize();  // Wait for the GPU to finish
    // Calculate and print the average distance from the origin
    float avgDistanceCudaMallocHost = 0.0f;
    for (int i = 0; i < numWalkers; i++)
    {
        avgDistanceCudaMallocHost += distancesCudaMallocHost[i];
    }
    avgDistanceCudaMallocHost /= static_cast<float>(numWalkers);
    // Clean up
    cudaFreeHost(distancesCudaMallocHost);
    auto stopCudaMallocHost = high_resolution_clock::now();
    // Calculate the elapsed time in milliseconds
    auto microsecondsCudaMallocHost = duration_cast<microseconds>(stopCudaMallocHost - startCudaMallocHost).count();




    // Start timer for cudaMallocManaged
    auto startCudaManaged = high_resolution_clock::now();
    // Allocate memory for distances using different methods
    float* distancesCudaManaged = nullptr;
    cudaMallocManaged((void**)&distancesCudaManaged, numWalkers * sizeof(float));
    calculateWithCudaManaged<<<numBlocks, blockSize>>>(numWalkers, numSteps, distancesCudaManaged, states);
    cudaDeviceSynchronize();  // Wait for the GPU to finish
    // Copy data from GPU
    float* distancesCudaManagedCPU = (float*)malloc(numWalkers * sizeof(float));
    cudaMemcpy(distancesCudaManagedCPU, distancesCudaManaged, numWalkers * sizeof(float), cudaMemcpyDeviceToHost);
    // Calculate and print the average distance from the origin
    float avgDistanceCudaManaged = 0.0f;

    for (int i = 0; i < numWalkers; i++)
    {
        avgDistanceCudaManaged += distancesCudaManagedCPU[i];
    }

    avgDistanceCudaManaged /= static_cast<float>(numWalkers);
    // Clean up
    cudaFree(distancesCudaManaged);
    free(distancesCudaManagedCPU);
    auto stopCudaManaged = high_resolution_clock::now();
    // Calculate the elapsed time in milliseconds
    auto microsecondsCudaManaged = duration_cast<microseconds>(stopCudaManaged - startCudaManaged).count();


    
    // Ensure that the GPU has finished its work before proceeding
    cudaDeviceSynchronize();

    // Print results
    std::cout << "Normal CUDA memory Allocation:" << std::endl;
    std::cout << "    Time to calculate (microsec): " << microsecondsCudaMalloc << std::endl;
    std::cout << "    Average distance from origin: " << avgDistanceCudaMalloc << std::endl;

    std::cout << "Pinned CUDA memory Allocation:" << std::endl;
    std::cout << "    Time to calculate (microsec): " << microsecondsCudaMallocHost << std::endl;
    std::cout << "    Average distance from origin: " << avgDistanceCudaMallocHost << std::endl;

    std::cout << "Managed CUDA memory Allocation:" << std::endl;
    std::cout << "    Time to calculate (microsec): " << microsecondsCudaManaged << std::endl;
    std::cout << "    Average distance from origin: " << avgDistanceCudaManaged << std::endl;
    std::cout << "Bye" << std::endl;

    // Free the individual rows first
    for (int i = 0; i < numWalkers; i++) {
        cudaFreeHost(states[i]);
    }

    // Then free the array of pointers
    cudaFreeHost(states);

    return 0;
}
