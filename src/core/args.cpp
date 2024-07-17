/**
 * @file args.cpp
 */

#include <exception>  // for std::exception
#include <string>     // for std::string

#include "args.hpp"

core::args::Args::Args(
    const int argc,
    char **argv)
{
    // Setup command-line argument parser (disable version and enable help only)
    argparse::ArgumentParser program("shogun", "", argparse::default_arguments::help);
    program.add_description("Learn Japanese kanji in the terminal.");

    // If "--kana" argument was passed, enable display of the kana transcriptions of the kanji
    program.add_argument("--kana")
        .help("display the kana transcription of the kanji")
        .store_into(this->display_kana);

    // If "--answer" argument was passed, enable display of the correct answer
    program.add_argument("--answer")
        .help("display the correct answer")
        .store_into(this->display_answer);

    // // If "--debug" argument was passed, enable debug mode
    // program.add_argument("--debug")
    //     .store_into(this->debug)
    //     .hidden();

    try {
        // Parse command-line arguments
        program.parse_args(argc, argv);
    }
    catch (const std::exception &err) {
        // Re-throw the exception as an ArgsParseError with the help message
        throw ArgsParseError(std::string(err.what()) + "\n\n" + program.help().str());
    }
}
