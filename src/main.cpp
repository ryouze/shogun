/**
 * @file main.cpp
 */

#include <cstdlib>    // for EXIT_FAILURE
#include <exception>  // for std::exception
#include <ios>        // for std::ios_base
#include <iostream>   // for std::cerr
#include <string>     // for std::string

#include "app.hpp"

/**
 * @brief Entry-point of the application.
 *
 * @param argc Number of command-line arguments (e.g., "2").
 * @param argv Array of command-line arguments (e.g., {"./bin", "-h"}).
 */
int main(int argc, char **argv)
{
    // Disable synchronization between the C++ standard streams (e.g., std::cin, std::cout) and their corresponding C equivalents
    std::ios_base::sync_with_stdio(false);

    try {
        // Parse command-line arguments, load the vocabulary JSON file from disk and start the interactive UI
        app::run(argc, argv);
    }
    catch (const std::exception &e) {
        std::cerr << std::string(e.what()) + '\n';
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Unknown error\n";
        return EXIT_FAILURE;
    }
    return 0;
}
