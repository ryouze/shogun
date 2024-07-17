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
 * @brief Check if the user's input is similar enough to the correct answer based on the global similarity threshold.
 *
 * @param user_input User's input (e.g., "to eat").
 * @param correct_answer Correct answer (e.g., "to eat").
 * @param min_similarity Minimum similarity between user input and correct answer (default: 0.6).
 *
 * @return True if the input matches the correct answer, otherwise false.
 *
 * @note If the initial check fails, the function will retry by stripping the correct answer up to the first comma and comparing again (e.g., "to eat, to drink" -> "to eat").
 */
[[nodiscard]] bool is_answer_correct(
    const std::string &user_input,
    const std::string &correct_answer,
    const double min_similarity = 0.6);

}  // namespace string
}  // namespace utils
