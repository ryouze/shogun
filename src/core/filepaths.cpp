/**
 * @file filepaths.cpp
 */

#include "filepaths.hpp"

#include <cstdint>     // for std::uint32_t
#include <filesystem>  // for std::filesystem
#include <stdexcept>   // for std::runtime_error
#include <string>      // for std::string
#include <vector>      // for std::vector

#if defined(_WIN32)
#include <windows.h>  // for GetModuleFileNameA
#elif defined(__APPLE__)
#include <mach-o/dyld.h>  // for _NSGetExecutablePath
#elif defined(__linux__)
#include <unistd.h>  // for readlink
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
[[nodiscard]] std::filesystem::path get_executable_path()
{
    std::vector<char> buffer(4096);  // Dynamically allocate buffer

#if defined(_WIN32)
    if (GetModuleFileNameA(NULL, buffer.data(), static_cast<DWORD>(buffer.size())) == 0) {
        throw PathError("Failed to get the executable path");
    }
#elif defined(__APPLE__)
    std::uint32_t size = static_cast<std::uint32_t>(buffer.size());
    if (_NSGetExecutablePath(buffer.data(), &size) != 0) {
        throw PathError("Failed to get the executable path");
    }
    return std::filesystem::canonical(buffer.data());
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
const std::filesystem::path root = get_executable_path().parent_path();

}  // namespace

// const std::string core::filepaths::state = (root / "state.json").string();
const std::string core::filepaths::vocabulary = (root / "vocabulary.json").string();
