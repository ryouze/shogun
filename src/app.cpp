/**
 * @file app.cpp
 */

#include <cstddef>  // for std::size_t
#include <string>   // for std::string, std::to_string
#include <vector>   // for std::vector

#include "app.hpp"
#include "core/filepaths.hpp"
#include "core/globals.hpp"
#include "io/kanji.hpp"
#include "utils/string.hpp"

#include <ftxui/component/component.hpp>           // for Component, CatchEvent, Input, Renderer, Container
#include <ftxui/component/screen_interactive.hpp>  // for ScreenInteractive
#include <ftxui/dom/elements.hpp>                  // for text, vbox, separator, border, size, center, bgcolor, bold, color, flex_grow

namespace {

/**
 * @brief Private helper struct that represents a single history entry.
 */
struct HistoryEntry {
    /**
     * @brief Construct a new HistoryEntry object.
     *
     * @param _number Number of the entry (e.g., "1").
     * @param _kanji Kanji (e.g., "三").
     * @param _kana Kana (e.g., "さん").
     * @param _translation English translation (e.g., "three").
     * @param _sentence_en English sentence (e.g., "She's the mother of three children.").
     * @param _is_correct Whether the user's input was correct (e.g., "true").
     */
    explicit HistoryEntry(const std::size_t _number,
                          const std::string &_kanji,
                          const std::string &_kana,
                          const std::string &_translation,
                          const std::string &_sentence_en,
                          const bool _is_correct)
        : number(_number),
          kanji(_kanji),
          kana(_kana),
          translation(_translation),
          sentence_en(_sentence_en),
          is_correct(_is_correct) {}

    /**
     * @brief Number of the entry (e.g., "1").
     */
    std::size_t number;

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
     * @brief English sentence (e.g., "She's the mother of three children.").
     */
    std::string sentence_en;

    /**
     * @brief Whether the user's input was correct (e.g., "true").
     */
    bool is_correct;
};

}  // namespace

void app::run()
{
    using namespace ftxui;

    std::string user_input;
    std::vector<HistoryEntry> history;
    std::size_t history_counter = 1;

    ScreenInteractive screen = ScreenInteractive::Fullscreen();

    // Load JSON file from disk
    auto vocab = io::kanji::Vocabulary(core::filepaths::vocabulary);

    // Get random entry from the vocabulary
    auto current_entry = vocab.get_entry();

    const auto input_component = Input(&user_input, "英語");

    const auto input_with_enter = CatchEvent(input_component, [&](const Event &event) {
        // If enter pressed, check if the user input is correct
        if (event == Event::Return) {
            const double similarity = utils::string::calculate_similarity(user_input, current_entry.translation);
            bool correct = similarity >= core::globals::min_similarity;  // 0.80

            // Fallback: If not correct, strip everything from the correct answer up to the first comma, then check again
            // This is useful when multiple translations are provided (e.g., "to eat, to drink"), we check against only the first translation (e.g., "to eat")
            if (!correct) {
                const std::size_t comma_pos = current_entry.translation.find(',');
                if (comma_pos != std::string::npos) {
                    const std::string stripped_translation = current_entry.translation.substr(0, comma_pos);
                    const double stripped_similarity = utils::string::calculate_similarity(user_input, stripped_translation);
                    correct = stripped_similarity >= core::globals::min_similarity;
                }
            }

            // Create a new HistoryEntry object and insert it into the history vector
            HistoryEntry newEntry(history_counter++, current_entry.kanji, current_entry.kana, current_entry.translation, current_entry.sentence_en, correct);
            history.insert(history.begin(), newEntry);

            if (history.size() > 5) {
                history.pop_back();
            }
            user_input.clear();
            current_entry = vocab.get_entry();
            return true;
        }
        return false;
    });

    const auto main_component = Container::Vertical({
        input_with_enter,
    });

    const auto main_renderer = Renderer(main_component, [&] {
        std::vector<Element> history_elements;
        for (const auto &entry : history) {
            history_elements.push_back(
                text(std::to_string(entry.number) + ". " + entry.kanji + (core::globals::display_kana ? "（" + entry.kana + "）" : "") + "= " + entry.translation + " (" + entry.sentence_en + ")") |
                color(entry.is_correct ? Color::Green : Color::Red));
        }
        while (history_elements.size() < 5) {
            history_elements.push_back(text(" "));
        }

        auto history_box = vbox(history_elements) | border | size(WIDTH, EQUAL, 90) | center;
        if (history.empty()) {
            history_box = text("履歴：なし") | center;
        }

        return vbox({
                   text("将軍") | center | bold,
                   separator(),
                   vbox({
                       text("漢字：" + current_entry.kanji + (core::globals::display_kana ? "（" + current_entry.kana + "）" : "") + (core::globals::display_answer ? "= " + current_entry.translation + "" : "")) | bold | size(WIDTH, EQUAL, 90),
                       text("例文：" + current_entry.sentence_jp) | bold | size(WIDTH, EQUAL, 90),
                       text("POS: " + current_entry.pos) | bold | size(WIDTH, EQUAL, 90),
                       input_with_enter->Render() | border | color(Color::Red),
                   }) | border |
                       center | size(WIDTH, EQUAL, 90) | flex_grow | hcenter,
                   separator(),
                   text("履歴") | bold | center,
                   history_box,
               }) |
               border | bgcolor(Color::Grey11) | center;
    });

    screen.Loop(main_renderer);
}
