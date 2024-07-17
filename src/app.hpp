/**
 * @file app.hpp
 *
 * @brief Main application logic.
 */

#pragma once

namespace app {

/**
 * @brief Parse command-line arguments, load the vocabulary JSON file from disk and start the interactive UI.
 *
 * @param argc Number of command-line arguments (e.g., "2").
 * @param argv Array of command-line arguments (e.g., {"./bin", "-h"}).
 *
 * @note Exceptions thrown by the JSON library (e.g., FileNotFoundError) are caught and displayed inside the UI loop.
 */
void run(
    const int argc,
    char **argv);

}  // namespace app
