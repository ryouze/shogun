/**
 * @file filepaths.cpp
 */

#include <filesystem>  // for std::filesystem
#include <string>      // for std::string

#include <cpplocate/cpplocate.h>

#include "filepaths.hpp"

namespace {

/**
 * @brief Helper variable containing the path to the directory containing the output executable (e.g., "~/GitHub/shogun/build").
 *
 * This variable is not accessible outside of this ".cpp" file.
 *
 * @details The return value of "cpplocate::getModulePath()" is a "std::string" that is immediately casted to a "std::filesystem::path".
 */
const std::filesystem::path root = cpplocate::getModulePath();

}  // namespace

const std::string core::filepaths::state = (root / "state.json").string();
const std::string core::filepaths::vocabulary = (root / "vocabulary.json").string();
