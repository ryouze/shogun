/**
 * @file main.cpp
 */

#include <cstdlib>    // for std::exit, EXIT_FAILURE
#include <exception>  // for std::exception
#include <ios>        // for std::ios_base
#include <iostream>   // for std::cerr
#include <string>     // for std::string

#include "app.hpp"

/**
 * @brief Entry-point of the application.
 */
int main()
{
    // Disable synchronization between the C++ standard streams (e.g., std::cin, std::cout) and their corresponding C equivalents
    std::ios_base::sync_with_stdio(false);

    try {
        app::run();
    }
    catch (const std::exception &e) {
        std::cerr << std::string(e.what()) + '\n';
        std::exit(EXIT_FAILURE);
    }
    catch (...) {
        std::cerr << "Unknown error\n";
        std::exit(EXIT_FAILURE);
    }
    return 0;
}
