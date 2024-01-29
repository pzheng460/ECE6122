#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cuda_runtime.h>
#include <curand.h>
#include <curand_kernel.h> // CUDA Random Number Generation

// Initialize CUDA random number generator
__global__
void initRandomState(unsigned long long seed, curandState_t* states, int numWalkers)
{
    int id = threadIdx.x + blockIdx.x * blockDim.x;
    if (id < numWalkers)
    {
        curand_init(seed, id, 0, &states[id]);
    }
}

// Function to perform calculations using cudaMalloc
__global__
void calculateWithCudaMalloc(int numWalkers, int numSteps, float* distances, curandState_t* states)
{
    int walkerId = blockIdx.x * blockDim.x + threadIdx.x;
    if (walkerId < numWalkers)
    {
        int x = 0;
        int y = 0;
        curandState_t localState = states[walkerId]; // Get the random state for this walker
        for (int i = 0; i < numSteps; i++)
        {
            // Simulate a random step in 2D space
            int direction = curand(&localState) % 4;
            if (direction == 0) x++;
            else if (direction == 1) x--;
            else if (direction == 2) y++;
            else y--;
        }
        distances[walkerId] = sqrt(static_cast<float>(x * x + y * y));
    }
}

// Function to perform calculations using cudaMallocHost
void calculateWithCudaMallocHost(int numWalkers, int numSteps, float* distances)
{
    // Similar to the cudaMalloc version, but no need for device kernel
    for (int walkerId = 0; walkerId < numWalkers; walkerId++)
    {
        int x = 0;
        int y = 0;
        for (int i = 0; i < numSteps; i++)
        {
            // Simulate a random step in 2D space
            int direction = rand() % 4;
            if (direction == 0) x++;
            else if (direction == 1) x--;
            else if (direction == 2) y++;
            else y--;
        }
        distances[walkerId] = sqrt(static_cast<float>(x * x + y * y));
    }
}

// Function to perform calculations using cudaMallocManaged
__global__
void calculateWithCudaManaged(int numWalkers, int numSteps, float* distances, curandState_t* states)
{
    int walkerId = blockIdx.x * blockDim.x + threadIdx.x;
    if (walkerId < numWalkers)
    {
        int x = 0;
        int y = 0;
        curandState_t localState = states[walkerId]; // Get the random state for this walker
        for (int i = 0; i < numSteps; i++)
        {
            // Simulate a random step in 2D space
            int direction = curand(&localState) % 4;
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
    if (argc != 5)
    {
        std::cerr << "Usage: " << argv[0] << " -W <Number of Walkers> -I <Number of Steps>" << std::endl;
        return 1;
    }

    int numWalkers = 0;
    int numSteps = 0;

    for (int i = 1; i < argc; i += 2)
    {
        std::string flag = argv[i];
        if (flag == "-W") numWalkers = std::atoi(argv[i + 1]);
        else if (flag == "-I") numSteps = std::atoi(argv[i + 1]);
    }

    if (numWalkers <= 0 || numSteps <= 0)
    {
        std::cerr << "Invalid input values. Number of walkers and steps must be positive integers." << std::endl;
        return 1;
    }

    // Allocate memory for distances using different methods
    float* distancesCudaMalloc = nullptr;
    float* distancesCudaMallocHost = nullptr;
    float* distancesCudaManaged = nullptr;
    curandState_t* randomStatesCudaMalloc = nullptr;
    curandState_t* randomStatesCudaManaged = nullptr;


    cudaMalloc((void**)&distancesCudaMalloc, numWalkers * sizeof(float));
    cudaMallocHost((void**)&distancesCudaMallocHost, numWalkers * sizeof(float));
    cudaMallocManaged((void**)&distancesCudaManaged, numWalkers * sizeof(float));
    cudaMalloc((void**)&randomStatesCudaMalloc, numWalkers * sizeof(curandState_t));
    cudaMalloc((void**)&randomStatesCudaManaged, numWalkers * sizeof(curandState_t));

    int blockSize = 256;
    int numBlocks = (numWalkers + blockSize - 1) / blockSize;

    // Initialize random seed
    srand(static_cast<unsigned int>(time(nullptr)));
    initRandomState<<<numBlocks, blockSize>>>(time(nullptr), randomStatesCudaMalloc, numWalkers);
    initRandomState<<<numBlocks, blockSize>>>(time(nullptr), randomStatesCudaManaged, numWalkers);

    // Measure time for cudaMalloc
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);
    calculateWithCudaMalloc<<<numBlocks, blockSize>>>(numWalkers, numSteps, distancesCudaMalloc, randomStatesCudaMalloc);
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    float millisecondsCudaMalloc = 0;
    cudaEventElapsedTime(&millisecondsCudaMalloc, start, stop);

    // Measure time for cudaMallocHost
    cudaEventRecord(start);
    calculateWithCudaMallocHost(numWalkers, numSteps, distancesCudaMallocHost);
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    float millisecondsCudaMallocHost = 0;
    cudaEventElapsedTime(&millisecondsCudaMallocHost, start, stop);

    // Measure time for cudaMallocManaged
    cudaEventRecord(start);
    calculateWithCudaManaged<<<numBlocks, blockSize>>>(numWalkers, numSteps, distancesCudaManaged, randomStatesCudaManaged);
    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    float millisecondsCudaManaged = 0;
    cudaEventElapsedTime(&millisecondsCudaManaged, start, stop);

    // Copy data from GPU
    float* distancesCudaMallocCPU = nullptr;
    float* distancesCudaManagedCPU = nullptr;
    distancesCudaMallocCPU = (float*)malloc(numWalkers * sizeof(float));
    distancesCudaManagedCPU = (float*)malloc(numWalkers * sizeof(float));

    cudaMemcpy(distancesCudaMalloc, distancesCudaMallocCPU, numWalkers * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(distancesCudaManaged, distancesCudaManagedCPU, numWalkers * sizeof(float), cudaMemcpyDeviceToHost);


    // Calculate and print the average distance from the origin
    float avgDistanceCudaMalloc = 0.0f;
    float avgDistanceCudaMallocHost = 0.0f;
    float avgDistanceCudaManaged = 0.0f;

    for (int i = 0; i < numWalkers; i++)
    {
        avgDistanceCudaMalloc += distancesCudaMallocCPU[i];
        avgDistanceCudaMallocHost += distancesCudaMallocHost[i];
        avgDistanceCudaManaged += distancesCudaManagedCPU[i];
    }

    avgDistanceCudaMalloc /= static_cast<float>(numWalkers);
    avgDistanceCudaMallocHost /= static_cast<float>(numWalkers);
    avgDistanceCudaManaged /= static_cast<float>(numWalkers);

    // Print results
    std::cout << "Normal CUDA memory Allocation:" << std::endl;
    std::cout << "    Time to calculate (microsec): " << millisecondsCudaMalloc << std::endl;
    std::cout << "    Average distance from origin: " << avgDistanceCudaMalloc << std::endl;

    std::cout << "Pinned CUDA memory Allocation:" << std::endl;
    std::cout << "    Time to calculate (microsec): " << millisecondsCudaMallocHost << std::endl;
    std::cout << "    Average distance from origin: " << avgDistanceCudaMallocHost << std::endl;

    std::cout << "Managed CUDA memory Allocation:" << std::endl;
    std::cout << "    Time to calculate (microsec): " << millisecondsCudaManaged << std::endl;
    std::cout << "    Average distance from origin: " << avgDistanceCudaManaged << std::endl;
    std::cout << "Bye" << std::endl;

    // Clean up
    cudaFree(distancesCudaMalloc);
    cudaFreeHost(distancesCudaMallocHost);
    cudaFree(distancesCudaManaged);
    cudaFree(randomStatesCudaMalloc);
    cudaFree(randomStatesCudaManaged);
    free(distancesCudaMallocCPU);
    free(distancesCudaManagedCPU);

    return 0;
}
