/**
 * @file json.cpp
 */

#include <exception>   // for std::exception
#include <filesystem>  // for std::filesystem
#include <fstream>     // for std::ifstream
#include <string>      // for std::string

#include "json.hpp"

core::json::Json core::json::load(const std::string &input_path)
{
    // Error: Doesn't exist
    if (!std::filesystem::exists(input_path)) {
        throw core::json::FileNotFoundError(input_path);
    }
    // Error: Is a directory
    else if (std::filesystem::is_directory(input_path)) {
        throw core::json::IsADirectoryError(input_path);
    }
    try {
        // Open the file in read mode
        std::ifstream file(input_path);

        // Error: File cannot be opened
        if (!file) {
            throw core::json::IOError(input_path);
        }

        // Return the parsed JSON
        return core::json::Json::parse(file);
    }
    catch (const core::json::Json::exception &e) {
        throw core::json::JsonParseError(input_path + " (" + std::string(e.what()) + ")");
    }
    catch (const std::exception &e) {
        throw core::json::IOError(input_path + " (" + std::string(e.what()) + ")");
    }
}

// void core::json::save(const core::json::Json &data,
//                       const std::string &output_path,
//                       const bool overwrite)
// {
//     // Error: Overwrite is false and the file already exists
//     // -> If overwrite is false, we don't want to overwrite the file, so raise the exception
//     // -> If overwrite is true, we can overwrite the file, keep going
//     if (!overwrite && std::filesystem::exists(output_path)) {
//         throw core::json::FileExistsError(output_path);
//     }
//     // Error: Is a directory
//     else if (std::filesystem::is_directory(output_path)) {
//         throw core::json::IsADirectoryError(output_path);
//     }
//     try {
//         // Open the file in write mode
//         std::ofstream file(output_path);

//         // Error: File cannot be opened
//         if (!file) {
//             throw core::json::IOError(output_path);
//         }

//         // Write the JSON object to the file with 4 spaces indentation and trailing newline
//         file << data.dump(4) << '\n';
//     }
//     catch (const core::json::Json::exception &e) {
//         throw core::json::JsonParseError(output_path + " (" + std::string(e.what()) + ")");
//     }
//     catch (const std::exception &e) {
//         throw core::json::IOError(output_path + " (" + std::string(e.what()) + ")");
//     }
// }
