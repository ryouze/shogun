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
const bool display_kana = true;

/**
 * @brief Display the correct answer.
 */
const bool display_answer = true;

/**
 * @brief Minimum similarity between user input and the correct answer as a percentage (0.0-1.0).
 */
const double min_similarity = 0.6;

}  // namespace globals
}  // namespace core
