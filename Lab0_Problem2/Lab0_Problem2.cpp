/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Aug 30, 2023

Description:

It's a program that continuously takes in a natural number from the console and outputs to the console all the numbers below the entered number that are multiples of 3 or 5 and then outputs the sum of all the multiples.

Entering a 0 ends the program.

*/
#include <iostream>
#include <string>
#include <sstream>
#include <cctype>

int main()
{
    while (true)
    {
        std::string input;
        int number; // Input number

        std::cout << "Please enter a natural number (0 to quit): ";
        std::getline(std::cin, input);

        // Check for valid input and handle quitting the program
        if (input == "0")
        {
            std::cout << "Program terminated." << std::endl;
            std::cout << "Have a nice day!" << std::endl;
            break;
        }

        bool isValid = true;
        for (char c : input)
        {
            if (!std::isdigit(c))
            {
                isValid = false;
                break;
            }
        }

        if (!isValid)
        {
            std::cout << "Invalid input. Please enter a valid natural number." << std::endl;
            continue;
        }

        std::stringstream ss(input);
        ss >> number;

        int sum = 0; // sum of all multiples
        bool found = false; // check if any multiple has been found

        std::cout << "The multiples of 3 below " << number << " are: ";
        for (int i = 3; i < number; i += 3)
        {
            // Check the multiples of 3
            if (found)
            {
                std::cout << ", ";
            }
            std::cout << i;
            sum += i;

            found = true;
        }
        if (!found)
        {
            std::cout << "None";
        }
        std::cout << "." << std::endl;

        found = false;
        std::cout << "The multiples of 5 below " << number << " are: ";
        for (int i = 5; i < number; i += 5)
        {
            // Check the multiples of 5
            if (i % 3 != 0)
            {
                if (found)
                {
                    std::cout << ", ";
                }
                std::cout << i;
                sum += i;
                found = true;
            }
        }
        if (!found)
        {
            std::cout << "None";
        }
        std::cout << "." << std::endl;

        std::cout << "The sum of all multiples is: " << sum << "." << std::endl;
    }

    return 0;
}
