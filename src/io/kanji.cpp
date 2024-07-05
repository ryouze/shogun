/**
 * @file kanji.cpp
 */

#include <algorithm>  // for std::shuffle
#include <random>     // for std::random_device, std::mt19937
#include <string>     // for std::string
#include <vector>     // for std::vector

#include "../core/json.hpp"
#include "kanji.hpp"

namespace {

/**
 * @brief Private helper function to load a JSON file containing Japanese vocabulary from disk.
 *
 * @param input_path Path to the JSON file (e.g., "~/data.json").
 * @param shuffle Whether to shuffle the data in random order (default: true).
 *
 * @return Vector of Entry objects.
 */
[[nodiscard]] std::vector<io::kanji::Entry> load_vocabulary(
    const std::string &input_path,
    const bool shuffle = true)
{
    // Initialize vector of Entry objects
    std::vector<io::kanji::Entry> entries;

    {
        // Load the JSON file from disk, passing any Exceptions to the caller
        const core::json::Json data = core::json::load(input_path);

        // Convert JSON to a vector of Entry objects

        for (auto &[key, value] : data.items()) {

            // Add the Entry object to the vector
            entries.emplace_back(io::kanji::Entry(
                key,  // Kanji
                value["kana"].get<std::string>(),
                value["translation"].template get<std::string>(),
                value["sentence_jp"].template get<std::string>(),
                value["sentence_en"].template get<std::string>(),
                value["pos"].template get<std::string>()));
        }
    }  // Deallocate the JSON object

    // If enabled, shuffle the data in random order
    if (shuffle) {
        std::random_device rd;
        std::mt19937 gen(rd());  // Mersenne Twister random number generator (32-bit)
        std::shuffle(entries.begin(), entries.end(), gen);
    }

    // Return shrunk vector (RVO)
    entries.shrink_to_fit();
    return entries;
}

}  // namespace

io::kanji::Vocabulary::Vocabulary(const std::string &file_path)
    : data_(load_vocabulary(file_path)),
      current_entry_(0)
{
}

const io::kanji::Entry &io::kanji::Vocabulary::get_entry()
{
    // If no more entries, start from the beginning
    if (this->current_entry_ >= this->data_.size()) {
        this->current_entry_ = 0;
    }

    return this->data_[this->current_entry_++];
}
