/**
 * @file filepaths.cpp
 */

#include <filesystem>  // for std::filesystem
#include <string>      // for std::string

#include <pathmaster/pathmaster.hpp>

#include "filepaths.hpp"

namespace {

/**
 * @brief Helper variable containing the path to the directory containing the output executable (e.g., "~/GitHub/shogun/build").
 *
 * This variable is not accessible outside of this ".cpp" file.
 */
const std::filesystem::path root = pathmaster::get_executable_path().parent_path();

}  // namespace

// const std::string core::filepaths::state = (root / "state.json").string();
const std::string core::filepaths::vocabulary = (root / "vocabulary.json").string();
