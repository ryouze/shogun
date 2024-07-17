/**
 * @file args.hpp
 *
 * @brief Process command-line arguments.
 */

#pragma once

#include <stdexcept>  // for std::runtime_error
#include <string>     // for std::string

#include <argparse/argparse.hpp>

namespace utils {
namespace args {

/**
 * @brief Base class for exceptions raised by command-line argument parser. A help message with usage, description, and examples must be included.
 */
class ArgsParseError : public std::runtime_error {
  public:
    explicit ArgsParseError(const std::string &message)
        : std::runtime_error(message) {}
};

/**
 * @brief Class that represents command-line arguments.
 *
 * On construction, the class processes the command-line arguments and sets the corresponding member variables.
 *
 * @note This class is marked as `final` to prevent inheritance.
 */
class Args final {
  public:
    /**
     * @brief Construct a new Args object.
     *
     * @param argc Number of command-line arguments (e.g., "2").
     * @param argv Array of command-line arguments (e.g., {"./bin", "-h"}).
     *
     * @throws ArgsParseError If failed to process command-line arguments. A help message with usage, description, and examples is returned.
     */
    explicit Args(const int argc,
                  char **argv);

    /**
     * @brief Display the kana transcription of the kanji (e.g., "true").
     *
     * @note This member is set based on the presence and value of the `--kana` command-line argument.
     */
    bool display_kana;

    /**
     * @brief Display the correct answer (e.g., "true").
     *
     * @note This member is set based on the presence and value of the `--answer` command-line argument.
     */
    bool display_answer;
};

}  // namespace args
}  // namespace utils
