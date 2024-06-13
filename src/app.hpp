/**
 * @file app.hpp
 *
 * @brief Main application logic.
 */

#pragma once

#include <stdexcept>  // for std::runtime_error
#include <string>     // for std::string

namespace app {

/**
 * @brief Base class for exceptions raised by command-line argument parser. A help message with usage, description, and examples must be included.
 */
class ArgParseError : public std::runtime_error {
  public:
    explicit ArgParseError(const std::string &message)
        : std::runtime_error(message) {}
};

/**
 * @brief Parse command-line arguments, load the vocabulary JSON file from disk and start the interactive UI.
 *
 * @param argc Number of command-line arguments (e.g., "2").
 * @param argv Array of command-line arguments (e.g., {"./bin", "-h"}).
 *
 * @throws ArgParseError If failed to process command-line arguments. A help message with usage, description, and examples is returned. Catch it inside main() and print the message.
 *
 * @note Other exceptions might be thrown depending on the underlying libraries.
 * @note The exceptions caused by the JSON library (e.g., FileNotFoundError) are caught and displayed inside the UI loop.
 */
void run(const int argc, char **argv);

}  // namespace app
