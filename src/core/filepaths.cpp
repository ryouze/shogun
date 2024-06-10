/**
 * @file filepaths.cpp
 */

#include "filepaths.hpp"

#include <filesystem>  // for std::filesystem
#include <stdexcept>   // for std::runtime_error
#include <string>      // for std::string
#include <vector>      // for std::vector

#if defined(_WIN32)
#include <windows.h>
#elif defined(__APPLE__)
#include <cstdint>
#include <mach-o/dyld.h>
#elif defined(__linux__)
#include <unistd.h>
#endif

namespace {

/**
 * @brief Base class for exceptions raised during file path operations.
 */
class PathError : public std::runtime_error {
  public:
    explicit PathError(const std::string &message)
        : std::runtime_error("PathError: " + message) {}
};

/**
 * @brief Helper function to get the path to the executable.
 *
 * @return Path to the executable (e.g., "~/GitHub/shogun/build/shogun").
 */
std::filesystem::path getExecutablePath()
{
    std::vector<char> buffer(4096);  // Dynamically allocate buffer

#if defined(_WIN32)
    if (GetModuleFileNameA(NULL, buffer.data(), buffer.size()) == 0) {
        throw PathError("Failed to get the executable path");
    }
#elif defined(__APPLE__)
    uint32_t size = static_cast<uint32_t>(buffer.size());
    if (_NSGetExecutablePath(buffer.data(), &size) != 0) {
        throw PathError("Failed to get the executable path");
    }
    const std::filesystem::path p(buffer.data());
    return std::filesystem::canonical(p);
#elif defined(__linux__)
    const ssize_t count = readlink("/proc/self/exe", buffer.data(), buffer.size());
    if (count == -1) {
        throw PathError("Failed to get the executable path");
    }
    buffer[count] = '\0';
#else
    throw PathError("Failed to get the executable path, unsupported platform");
#endif

    return std::filesystem::canonical(buffer.data());
}

/**
 * @brief Helper variable containing the path to the directory containing the output executable (e.g., "~/GitHub/shogun/build").
 *
 * This variable is not accessible outside of this ".cpp" file.
 */
const std::filesystem::path root = getExecutablePath().parent_path();

}  // namespace

const std::string core::filepaths::state = (root / "state.json").string();
const std::string core::filepaths::vocabulary = (root / "vocabulary.json").string();
