/**
 * @file string.hpp
 *
 * @brief String operations.
 */

#pragma once

#include <string>  // for std::string

namespace utils {
namespace string {

/**
 * @brief Calculate the similarity between two strings using the Levenshtein distance.
 *
 * @param str1 First string (e.g., "HELLO World!").
 * @param str2 Second string (e.g., "hello world").
 *
 * @return Similarity score between 0.0 and 1.0 (e.g., "0.916667").
 *
 * @note Trailing whitespace will be removed and strings will be converted to lowercase before comparison.
 */
[[nodiscard]] double calculate_similarity(const std::string &str1,
                                          const std::string &str2);

}  // namespace string
}  // namespace utils
