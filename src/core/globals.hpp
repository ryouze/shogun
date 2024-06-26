/**
 * @file globals.hpp
 *
 * @brief Global variables.
 */

#pragma once

namespace core {
namespace globals {

/**
 * @brief Display the kana transcription of the kanji.
 */
bool display_kana = true;

/**
 * @brief Display the correct answer.
 */
bool display_answer = true;

/**
 * @brief Minimum similarity between user input and the correct answer as a percentage (0.0 - 1.0).
 */
constexpr double min_similarity = 0.6;

}  // namespace globals
}  // namespace core
