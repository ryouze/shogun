/**
 * @file string.cpp
 */

#include <algorithm>    // for std::transform, std::min, std::swap, std::max
#include <cctype>       // for std::tolower
#include <cstddef>      // for std::size_t
#include <string>       // for std::string
#include <string_view>  // for std::string_view
#include <vector>       // for std::vector

#include "string.hpp"

namespace {

/**
 * @brief Private helper function to remove leading and trailing whitespace from a string.
 *
 * @param str String to trim (e.g., "  hello  ").
 *
 * @return Trimmed string (e.g., "hello").
 */
[[nodiscard]] std::string trim_whitespace(const std::string &str)
{
    // Define the whitespace characters to remove
    static constexpr std::string_view whitespace = " \t\n\r\f\v";

    // Find the first non-whitespace character in the string
    const std::size_t start_pos = str.find_first_not_of(whitespace);

    // If the string is all whitespace, return an empty string
    if (start_pos == std::string::npos) {
        return "";
    }

    // Find the last non-whitespace character in the string
    const std::size_t end_pos = str.find_last_not_of(whitespace);

    // Return string without leading and trailing whitespace
    return str.substr(start_pos, end_pos - start_pos + 1);
}

/**
 * @brief Private helper function to convert a string to lowercase.
 *
 * @param str String to convert to lowercase (e.g., "Hello World").
 *
 * @return Lowercase string (e.g., "hello world").
 */
[[nodiscard]] std::string to_lower(const std::string &str)
{
    // Create copy of the input string (so we can use RVO when returning the result)
    std::string lower_str = str;

    // Convert the string to lowercase
    std::transform(lower_str.cbegin(), lower_str.cend(), lower_str.begin(),
                   // Since std::to_lower expects an "int", we cast "char" to "unsigned char"
                   [](unsigned char c) { return std::tolower(c); });

    // Return the lowercase string (RVO)
    return lower_str;
}

/**
 * @brief Private helper function to calculate the similarity between two strings using the Levenshtein distance.
 *
 * @param str1 First string (e.g., "HELLO World!").
 * @param str2 Second string (e.g., "hello world").
 *
 * @return Similarity score between 0.0 and 1.0 (e.g., "0.916667").
 *
 * @note Trailing whitespace will be removed and strings will be converted to lowercase before comparison.
 */
[[nodiscard]] double calculate_similarity(
    const std::string &str1,
    const std::string &str2)
{
    // Remove trailing whitespace and turn lowercase for case-insensitive comparison
    const std::string str1_lower = to_lower(trim_whitespace(str1));
    const std::string str2_lower = to_lower(trim_whitespace(str2));

    // Get the lengths of the lowercase strings
    const std::size_t str1_lower_len = str1_lower.size();
    const std::size_t str2_lower_len = str2_lower.size();

    // If either string is empty, return 1.0 if both are empty, 0.0 otherwise
    if (str1_lower_len == 0) {
        return str2_lower_len == 0 ? 1.0 : 0.0;
    }
    if (str2_lower_len == 0) {
        return 0.0;
    }

    // Initialize two rows for the Levenshtein distance calculation
    std::vector<std::size_t> previous_row(str2_lower_len + 1);
    std::vector<std::size_t> current_row(str2_lower_len + 1);

    // Fill the first row with increasing numbers
    for (std::size_t j = 0; j <= str2_lower_len; ++j) {
        previous_row[j] = j;
    }

    // Compute the Levenshtein distance
    for (std::size_t i = 1; i <= str1_lower_len; ++i) {
        current_row[0] = i;
        for (std::size_t j = 1; j <= str2_lower_len; ++j) {
            // Calculate the cost of substitution
            std::size_t substitution_cost = str1_lower[i - 1] == str2_lower[j - 1] ? 0 : 1;
            // Assign the minimum of deletion, insertion, or substitution cost
            current_row[j] = std::min({previous_row[j] + 1, current_row[j - 1] + 1, previous_row[j - 1] + substitution_cost});
        }
        // Swap current and previous rows for next iteration
        std::swap(previous_row, current_row);
    }

    // Compute the similarity score based on the Levenshtein distance and the length of the longest string
    const std::size_t lev_distance = previous_row[str2_lower_len];
    return 1.0 - static_cast<double>(lev_distance) / static_cast<double>(std::max(str1_lower_len, str2_lower_len));
}

}  // namespace

bool utils::string::is_answer_correct(
    const std::string &user_input,
    const std::string &correct_answer,
    const double min_similarity)
{
    // Calculate the similarity between the user's input and the correct answer using the Levenshtein distance
    const double similarity = calculate_similarity(user_input, correct_answer);

    // If the similarity is above the threshold, mark the answer as correct
    bool correct = similarity >= min_similarity;

    // Fallback: If not correct, strip everything from the correct answer up to the first comma and check again
    // This is useful when multiple translations are provided (e.g., "to eat, to drink"); we only check the first translation (e.g., "to eat")
    if (!correct) {
        const std::size_t comma_pos = correct_answer.find(',');
        if (comma_pos != std::string::npos) {
            const std::string stripped_translation = correct_answer.substr(0, comma_pos);
            const double stripped_similarity = calculate_similarity(user_input, stripped_translation);
            correct = stripped_similarity >= min_similarity;
        }
    }

    return correct;
}
