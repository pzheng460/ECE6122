/*
Author: Peizhen Zheng
Class: ECE6122 (A)
Last Date Modified: Aug 30, 2023

Description:

It's a program a C++ program using the insertion stream operator and escape sequences that outputs the text to your terminal screen when executed.

*/
#include <iostream>

int main() {
    std::cout << "My name is: Peizhen Zheng\n"
              << "This (\") is a double quote.\n"
              << "This (') is a single quote.\n"
              << "This (\\) is a backslash.\n"
              << "This (/) is a forward slash.\n";
    return 0;
}
