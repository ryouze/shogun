/**
 * @file app.cpp
 */

#include <atomic>   // for std::atomic
#include <cstddef>  // for std::size_t
#include <memory>   // for std::unique_ptr, std::make_unique
#include <string>   // for std::string, std::to_string
#include <thread>   // for std::thread
#include <utility>  // for std::move
#include <vector>   // for std::vector

#include "app.hpp"
#include "core/filepaths.hpp"
#include "core/globals.hpp"
#include "io/kanji.hpp"
#include "utils/string.hpp"

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

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

    // Initialize variables
    std::atomic<bool> is_loading = true;           // Atomic allows for thread-safe access
    std::unique_ptr<io::kanji::Vocabulary> vocab;  // Must be a pointer to allow for initialization in a separate thread
    io::kanji::Entry current_entry;
    std::string user_input;
    std::vector<HistoryEntry> history;
    std::size_t history_counter = 1;

    // Define screen to be fullscreen
    ScreenInteractive screen = ScreenInteractive::Fullscreen();

    // Define loading UI
    auto loading_renderer = Renderer([&] {
        return vbox({
                   // Title
                   text("将軍") | center | bold,
                   separator(),
                   // Loading JSON: <filepath>
                   vbox({
                       text("Loading JSON: " + core::filepaths::vocabulary) | bold | center,
                   }) |
                       center | size(WIDTH, EQUAL, 90) | flex_grow | hcenter,
               }) |
               border | bgcolor(Color::Grey11) | center;
    });

    // Define user input component and event handler
    auto input_component = Input(&user_input, "英語");
    auto input_with_enter = CatchEvent(input_component, [&](const Event &event) {
        // If not loading and the user presses Enter, check the answer
        if (event == Event::Return && !is_loading) {

            // Check how similar the user's input is to the correct answer
            const double similarity = utils::string::calculate_similarity(user_input, current_entry.translation);
            // If the similarity is above the threshold, mark the answer as correct
            bool correct = similarity >= core::globals::min_similarity;

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

            // Insert the new history entry at the beginning of the history vector
            history.emplace(history.begin(), history_counter++, current_entry.kanji, current_entry.kana, current_entry.translation, current_entry.sentence_en, correct);

            // If the history vector is longer than 5 entries, remove the last entry to keep the history size at 5
            if (history.size() > 5) {
                history.pop_back();
            }

            // Clear the user's input
            user_input.clear();

            // Get a new random entry from the vocabulary
            current_entry = vocab->get_entry();
            return true;
        }

        return false;
    });

    // Main renderer for the main application screen
    auto main_renderer = Renderer([&] {
        // If loading, render the loading screen
        if (is_loading) {
            return loading_renderer->Render();
        }

        // Otherwise, render the main application screen

        // Create UI elements for the history dynamically
        std::vector<Element> history_elements;
        history_elements.reserve(5);  // Reserve space for 5 elements
        for (const auto &entry : history) {
            history_elements.emplace_back(
                text(std::to_string(entry.number) + ". " + entry.kanji + (core::globals::display_kana ? "（" + entry.kana + "）" : "") + "= " + entry.translation + " (" + entry.sentence_en + ")") |
                color(entry.is_correct ? Color::Green : Color::Red));
        }
        while (history_elements.size() < 5) {
            history_elements.emplace_back(text(" "));
        }

        auto history_box = vbox(std::move(history_elements)) | border | size(WIDTH, EQUAL, 90) | center;

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

    // Run the loading thread
    std::thread load_thread([&]() {
        // Load JSON file from disk
        vocab = std::make_unique<io::kanji::Vocabulary>(core::filepaths::vocabulary);
        // Get random entry from the vocabulary
        current_entry = vocab->get_entry();
        // Set loading flag to false and post a custom event to trigger the screen update
        is_loading = false;
        screen.PostEvent(Event::Custom);
    });

    // Detach the thread to avoid blocking
    load_thread.detach();

    // Main loop with a container to handle focus
    auto main_container = Container::Vertical({
        input_with_enter,
    });

    auto main_app = Renderer(main_container, [&] {
        return main_renderer->Render();
    });

    screen.Loop(main_app);
}
