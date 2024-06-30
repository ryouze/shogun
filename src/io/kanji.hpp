/**
 * @file kanji.hpp
 *
 * @brief Load Japanese kanji from disk and return random entries.
 */

#pragma once

#include <cstddef>  // for std::size_t
#include <string>   // for std::string
#include <vector>   // for std::vector

#include "../core/json.hpp"

namespace io {
namespace kanji {

/**
 * @brief Struct that represents a single Japanese kanji entry (kanji, kana, translation, Japanese sentence, English sentence, part of speech).
 */
struct Entry {
    /**
     * @brief Construct a new Entry object.
     *
     * This is the default constructor, which does not initialize any member variables.
     */
    Entry() = default;

    /**
     * @brief Construct a new Entry object.
     *
     * @param _kanji Kanji (e.g., "三").
     * @param _kana Kana (e.g., "さん").
     * @param _translation English translation (e.g., "three").
     * @param _sentence_jp Japanese sentence (e.g., "彼女[かのじょ]は三[さん]人[にん]の子供[こども]の母親[ははおや]だ。").
     * @param _sentence_en English sentence (e.g., "She's the mother of three children.").
     * @param _pos Part of speech (e.g., "Noun").
     */
    explicit Entry(const std::string &_kanji,
                   const std::string &_kana,
                   const std::string &_translation,
                   const std::string &_sentence_jp,
                   const std::string &_sentence_en,
                   const std::string &_pos)
        : kanji(_kanji),
          kana(_kana),
          translation(_translation),
          sentence_jp(_sentence_jp),
          sentence_en(_sentence_en),
          pos(_pos) {}

    /**
     * @brief Japanese kanji (e.g., "三").
     */
    std::string kanji;

    /**
     * @brief Japanese kana (e.g., "さん").
     */
    std::string kana;

    /**
     * @brief English translation (e.g., "three").
     */
    std::string translation;

    /**
     * @brief Japanese sentence (e.g., "彼女[かのじょ]は三[さん]人[にん]の子供[こども]の母親[ははおや]だ。").
     */
    std::string sentence_jp;

    /**
     * @brief English sentence (e.g., "She's the mother of three children.").
     */
    std::string sentence_en;

    /**
     * @brief Part of speech (e.g., "Noun").
     */
    std::string pos;
};

/**
 * @brief Class that represents a vocabulary of Japanese kanji.
 *
 * On construction, the class loads and randomly shuffles a JSON file containing Japanese vocabulary from disk.
 *
 * @note This class is marked as `final` to prevent inheritance.
 */
class Vocabulary final {
  public:
    /**
     * @brief Construct a new Vocabulary object.
     *
     * @param file_path Path to the file to load (e.g., "vocabulary.json").
     */
    explicit Vocabulary(const std::string &file_path);

    /**
     * @brief Get a single kanji entry from the vocabulary until exhaustion, then loop back to the beginning.
     *
     * @return Entry object (kanji, kana, translation, Japanese sentence, English sentence, part of speech).
     */
    [[nodiscard]] const Entry &get_entry();

  private:
    /**
     * @brief Vector of Entry objects.
     */
    const std::vector<Entry> data_;

    /**
     * @brief Index of the current entry.
     *
     * This is used to keep track of the current entry in the vector, so that we can loop back to the beginning when we reach the end.
     */
    std::size_t current_entry_;
};

}  // namespace kanji
}  // namespace io
