/**
 * @file json.hpp
 *
 * @brief Load and save JSON files.
 */

#pragma once

#include <stdexcept>  // for std::runtime_error
#include <string>     // for std::string

#include <nlohmann/json.hpp>

namespace core {
namespace json {

/**
 * @brief Alias for "nlohmann::json".
 *
 * @note Documentation: https://github.com/nlohmann/json
 */
using json = nlohmann::json;

/**
 * @brief Base class for exceptions raised during I/O operations.
 */
class IOError : public std::runtime_error {
  public:
    explicit IOError(const std::string &message)
        : std::runtime_error(message) {}
};

/**
 * @brief Raised when a file does not exist.
 */
class FileNotFoundError : public IOError {
  public:
    explicit FileNotFoundError(const std::string &message)
        : IOError("FileNotFoundError: " + message) {}
};

// /**
//  * @brief Raised when a file already exists.
//  */
// class FileExistsError : public IOError {
//   public:
//     explicit FileExistsError(const std::string &message)
//         : IOError("FileExistsError: " + message) {}
// };

/**
 * @brief Raised when a JSON file cannot be parsed.
 */
class JsonParseError : public IOError {
  public:
    explicit JsonParseError(const std::string &message)
        : IOError("JsonParseError: " + message) {}
};

/**
 * @brief Load a JSON file from disk.
 *
 * @param input_path Path to the JSON file (e.g., "~/data.json").
 *
 * @return JSON object.
 *
 * @throws core::json::FileNotFoundError If the file does not exist.
 * @throws core::json::JsonParseError If the file cannot be parsed as a JSON object.
 * @throws core::json::IOError If the file cannot be opened for reading or if any other I/O error occurs.
 *
 * @note You can catch all exceptions listed above with core::json::IOError.
 */
[[nodiscard]] json load(const std::string &input_path);

// /**
//  * @brief Save a JSON object to a file on disk.
//  *
//  * @param data JSON object to save.
//  * @param output_path Path to the JSON file (e.g., "~/data.json").
//  * @param overwrite Overwrite the file if it already exists (default: false).
//  *
//  * @throws core::json::FileExistsError If overwrite is false and the file already exists.
//  * @throws core::json::JsonParseError If the JSON object cannot be serialized.
//  * @throws core::json::IOError If the file cannot be opened for writing or if any other I/O error occurs.
//  *
//  * @note You can catch all exceptions listed above with core::json::IOError.
//  */
// void save(
//     const json &data,
//     const std::string &output_path,
//     const bool overwrite = false);

}  // namespace json
}  // namespace core
