/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Sep 17, 2023

Description:

Given an 2D grid of N x M point charges, all with the same charge q, in the x-y plane, centered around the origin, this program is to compute the magnitude and direction of the electric field at a specific point (x, y, z) in space due to all the charges.

Enter a "Y" to repeat the calculation, while enter a "N" to terminate the program.

*/

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <mutex>
#include <condition_variable>

#include "ECE_ElectricField.h"

using namespace std;

mutex mtx; // mutex variable
condition_variable cv; // condition variable
int finishedThreads = 0; // finished threads
bool startCalculation = false; // determine whether to start the calculation
bool allThreadsFinished = false; // determine whether all the threads have finished
double totalEx = 0.0, totalEy = 0.0, totalEz = 0.0; // Initialize the total electric field components
double x = 0.0, y = 0.0, z = 0.0; // Location at which to calculate the electric field
double q; // Charge of each point charge
unsigned int numThreads; // the number of threads
bool terminateAll = false; // determine whether to terminate all
int repeatCount = 0; // count the repeat times

// get the final result, transfer from "xxxxe+xxx" to "xxxx * 10^x"
string getFinalResult(double number)
{
    // if number is zero, we don't need to compute
    if (number == 0)
    {
        return "0.0000 * 10^0";
    }

    // Calculate the exponent (y)
    int exponent = static_cast<int>(floor(log10(std::abs(number))));

    // Calculate the mantissa (x)
    double mantissa = number / pow(10, exponent);

    // Convert x and y to a formatted string
    ostringstream formatted;
    formatted << fixed << setprecision(4);
    formatted << mantissa << " * 10^" << exponent;

    return formatted.str();
}

// split the input by ' '
bool split(const string& s, char delimiter, vector<string>& results)
{
    bool bRC = true;
    if (s.empty())
    {
        bRC = false;
    }
    else
    {
        string token;
        istringstream tokenStream(s);
        while (getline(tokenStream, token, delimiter))
        {
            results.push_back(token);
        }
    }
    return bRC;
}

// check if the input is float or not
bool isFloat(const string& str)
{
    if (str.empty())
    {
        return false;
    }

    std::istringstream iss(str);
    float value;
    char leftover;

    // Try to parse the entire string as a float, and check for leftover characters
    if (!(iss >> value) || (iss >> leftover))
    {
        return false;  // Failed to parse as a float or leftover characters found
    }

    return true;  // Successfully parsed as a float
}

// thread function to compute the results
void threadFunction(int threadId, const vector<vector<double>>& points)
{
    int repeatCountThread = 0;
    do
    {
        // wait until the main function tell the thread to start
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [] { return startCalculation; });
        }

        // calculate all and sum them together
        double sumEx = 0.0, sumEy = 0.0, sumEz = 0.0;
        for (auto point : points)
        {
            double localEx, localEy, localEz;
            ECE_ElectricField electricField(point[0], point[1], 0, q);
            electricField.computeFieldAt(x, y, z);
            electricField.getElectricField(localEx, localEy, localEz);
            {
                sumEx += localEx;
                sumEy += localEy;
                sumEz += localEz;
            }
        }

        // add the value to the global variables
        {
            unique_lock<mutex> lock(mtx);
            totalEx += sumEx;
            totalEy += sumEy;
            totalEz += sumEz;
//            cout << "Thread " << threadId << " finished computation." << endl;
            // after every thread finished running, notify the main function
            finishedThreads++;

            if (finishedThreads == numThreads - 1)
            {
//                cout << "all thread finished" << endl;
                allThreadsFinished = true;
                cv.notify_all();
            }
        }

        // wait until the main function tell the thread to continue
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [repeatCountThread] { return repeatCount != repeatCountThread; });
        }
        repeatCountThread++;
        // terminate the function
        if (terminateAll)
        {
            break;
        }
    } while (repeatCount == repeatCountThread);
}

int main() {
    // Determine the number of concurrent threads
    numThreads = thread::hardware_concurrency();
    cout << "Your computer supports " << numThreads << " concurrent threads." << endl;

    int N, M; // Number of rows and columns in the grid
    double xSep, ySep; // Separation distances in x and y directions

    char delimiter = ' '; // delimiter
    stringstream ss;

    // input the N * M, and check if they are natural numbers
    while (true)
    {
        string input;
        cout << "Please enter the number of rows and columns in the N x M array: ";
        getline(cin, input);
        vector<string> results;
        bool isValid;

        isValid = split(input, delimiter, results);

        // check the size of results
        if (results.size() != 2)
        {
            isValid = false;
        }

        // if it's valid, then check the digit
        if (isValid)
        {
            for (const auto& number : results)
            {
                for (char c : number)
                {
                    if (!isdigit(c))
                    {
                        isValid = false;
                        break;
                    }
                }
                if (!isValid)
                {
                    break;
                }
            }
        }

        if (!isValid)
        {
            cout << "Invalid input!" << endl;
            continue;
        }

        // read the results
        ss << results[0];
        ss >> N;
        ss.clear();
        ss << results[1];
        ss >> M;
        ss.clear();

        if (N <= 0 || M <= 0)
        {
            cout << "Invalid input!" << endl;
            continue;
        }
        break;
    }

    // input the x and y separation distances in meters
    while (true)
    {
        string input;
        cout << "Please enter the x and y separation distances in meters: ";
        getline(cin, input);
        vector<string> results;
        bool isValid;

        isValid = split(input, delimiter, results);

        if (results.size() != 2)
        {
            isValid = false;
        }

        // Check if the results are floats
        if (isValid)
        {
            for (const auto& number : results)
            {
                if (!isFloat(number))
                {
                    isValid = false;
                    break;
                }
            }
        }

        if (!isValid)
        {
            cout << "Invalid input!" << endl;
            continue;
        }

        ss << results[0];
        ss >> xSep;
        ss.clear();
        ss << results[1];
        ss >> ySep;
        ss.clear();

        if (xSep <= 0 || ySep <= 0)
        {
            cout << "Invalid input!" << endl;
            continue;
        }
        break;
    }

    // input the common charge on the points in micro C
    while (true)
    {
        string input;
        cout << "Please enter the common charge on the points in micro C: ";
        getline(cin, input);
        vector<string> results;
        bool isValid;

        isValid = split(input, delimiter, results);

        if (results.size() != 1)
        {
            isValid = false;
        }

        // Check if the results are floats
        if (isValid)
        {
            for (const auto& number : results)
            {
                if (!isFloat(number))
                {
                    isValid = false;
                    break;
                }
            }
        }

        if (!isValid)
        {
            cout << "Invalid input!" << endl;
            continue;
        }

        ss << results[0];
        ss >> q;
        ss.clear();
        q = q * 1e-6;
        break;
    }

    double xLeft = - (N - 1) * xSep / 2;
    double yBottom = - (M - 1) * ySep / 2;

    // divide the points into numThreads - 1 groups
    vector<vector<vector<double>>> points(numThreads - 1);
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            int threadId = (j * N + i + 1) % (numThreads - 1);
            double localX = xLeft + i * xSep;
            double localY = yBottom + j * ySep;
            vector<double> point = {localX, localY};
            points[threadId].push_back(point);
        }
    }

    // establish multithreading
    vector<thread> threads;
    for (int i = 0; i < numThreads - 1; i++)
    {
        threads.emplace_back(threadFunction, i, points[i]);
    }

    // Loop for continuous calculations
    char repeat;
    do
    {
        cout << endl;

        while (true)
        {
            string input;
            cout << "Please enter the location in space to determine the electric field (x y z) in meters: ";
            getline(cin, input);
            vector<string> results;
            bool isValid;

            isValid = split(input, delimiter, results);

            if (results.size() != 3)
            {
                isValid = false;
            }

            // Check if results are floats
            if (isValid)
            {
                for (const auto& number : results)
                {
                    if (!isFloat(number))
                    {
                        isValid = false;
                        break;
                    }
                }
            }

            if (!isValid)
            {
                cout << "Invalid input!" << endl;
                continue;
            }

            ss << results[0];
            ss >> x;
            ss.clear();
            ss << results[1];
            ss >> y;
            ss.clear();
            ss << results[2];
            ss >> z;
            ss.clear();

            // Check if the location matches a point charge position
            if (z == 0)
            {
                if (fmod(x - xLeft, xSep) == 0.0 && fmod(y - yBottom, ySep) == 0.0)
                {
                    cout << "Invalid location! Location matches a point charge position!" << endl;
                    continue;
                }
            }

            break;
        }

        // Measure the time taken for calculation
        auto startTime = chrono::high_resolution_clock::now();

        // main thread notifies other threads to start their works
        {
            unique_lock<mutex> lock(mtx);
            startCalculation = true;
            cv.notify_all();
        }

        // waiting for other threads to finish their works
        {
            unique_lock<mutex> lock(mtx);
            cv.wait(lock, [] { return allThreadsFinished; });
            startCalculation = false;
            allThreadsFinished = false;
            finishedThreads = 0;
        }

        // Calculate the magnitude of the total electric field
        double totalE = sqrt(totalEx * totalEx + totalEy * totalEy + totalEz * totalEz);

        // Get the time from the user inputs the xyz location to just before the results are output to the screen.
        auto endTime = chrono::high_resolution_clock::now();
        chrono::nanoseconds duration = chrono::duration_cast<chrono::nanoseconds>(endTime - startTime);

        startCalculation = false;
//        cout << "startCalculation changed" << endl;

        // Output the results, and set precision to 4 decimal places
        cout << "The electric field at (" << fixed << setprecision(1) << x << ", " << fixed << setprecision(1) << y << ", " << setprecision(1) << z << ") in V/m is:" << endl;
        cout << "Ex = " << getFinalResult(totalEx) << endl;
        cout << "Ey = " << getFinalResult(totalEy) << endl;
        cout << "Ez = " << getFinalResult(totalEz) << endl;
        cout << "|E| = " << getFinalResult(totalE) << endl;
        cout << "The calculation took " << fixed << setprecision(4) << static_cast<double>(duration.count()) / 1000 << " microsec!" << endl;

        // Ask if the user wants to repeat the calculation
        cout << "Do you want to enter a new location (Y/N)? ";

        while (!(cin >> repeat) || (repeat != 'Y' && repeat != 'N'))
        {
            cout << "Invalid input!" << endl;
            cout << "Do you want to enter a new location (Y/N)? ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin.ignore();
        // Initialize the total electric field components
        totalEx = 0.0, totalEy = 0.0, totalEz = 0.0;
        if (repeat == 'Y')
        {
            unique_lock<mutex> lock(mtx);
            repeatCount += 1;
            cv.notify_all();
        }
    } while (repeat == 'Y');

    cout << "Bye!" << endl;
    // Wait for all threads to finish
    {
        unique_lock<mutex> lock(mtx);
        terminateAll = true;
        repeatCount++;
        cv.notify_all();
    }
    for (auto& thread : threads)
    {
        thread.join();
    }

    return 0;
}
