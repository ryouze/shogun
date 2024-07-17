/**
 * @file app.cpp
 */

#include <atomic>     // for std::atomic
#include <cstddef>    // for std::size_t
#include <exception>  // for std::exception_ptr, std::rethrow_exception, std::exception, std::current_exception
#include <memory>     // for std::unique_ptr, std::make_unique
#include <string>     // for std::string, std::to_string
#include <thread>     // for std::thread
#include <utility>    // for std::move
#include <vector>     // for std::vector

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include "app.hpp"
#include "core/args.hpp"
#include "core/filepaths.hpp"
#include "io/kanji.hpp"
#include "utils/string.hpp"

namespace {

/**
 * @brief Private helper enum that represents the state of the hint display.
 *
 * This enum is used to control the visibility and amount of hint information displayed to the user.
 */
enum class HintState : unsigned char {
    /**
     * @brief No hint is displayed.
     */
    Off,

    /**
     * @brief Only the kana transcription is displayed as a hint.
     */
    Partial,

    /**
     * @brief Both the kana transcription and the correct answer are displayed as hints.
     */
    Full
};

/**
 * @struct Private helper struct that represents a single history entry.
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
    explicit HistoryEntry(
        const std::size_t _number,
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

void app::run(
    const int argc,
    char **argv)
{
    // Use ftxui namespace because we're using a lot of ftxui components
    using namespace ftxui;

    // Process command-line arguments (this might throw a ArgParseError)
    const core::args::Args args = core::args::Args(argc, argv);

    // Initialize variables
    std::atomic<bool> is_loading = true;           // Atomic allows for thread-safe access
    std::unique_ptr<io::kanji::Vocabulary> vocab;  // Must be a pointer to allow for initialization in a separate thread
    std::exception_ptr load_exception;             // Exception pointer to capture any exceptions thrown during loading of vocab
    io::kanji::Entry current_entry;
    std::string user_input;
    std::vector<HistoryEntry> history;
    std::size_t history_counter = 1;

    // Initialize hint flags
    bool display_kana = args.display_kana;
    bool display_answer = args.display_answer;
    HintState help_hint = HintState::Off;

    // Define screen to be fullscreen
    ScreenInteractive screen = ScreenInteractive::Fullscreen();

    // Define loading UI
    const auto loading_renderer = Renderer([&] {
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
    const auto input_component = Input(&user_input, "英語");
    const auto input_with_enter = CatchEvent(input_component, [&](const Event &event) {
        // If loading, do nothing
        if (is_loading) {
            return false;
        }
        // If user presses tab, toggle between off, showing kana, showing both kana and answer
        else if (event == Event::Tab) {
            switch (help_hint) {
            case HintState::Off:  // Off -> Partial
                help_hint = HintState::Partial;
                display_kana = true;
                display_answer = args.display_answer;  // User preference
                break;
            case HintState::Partial:  // Partial -> Full
                help_hint = HintState::Full;
                display_kana = true;
                display_answer = true;
                break;
            case HintState::Full:  // Full -> Off
                help_hint = HintState::Off;
                display_kana = args.display_kana;      // User preference
                display_answer = args.display_answer;  // User preference
                break;
            default:  // Invalid -> Off
                help_hint = HintState::Off;
                display_kana = args.display_kana;      // User preference
                display_answer = args.display_answer;  // User preference
                break;
            }
            return true;
        }
        // If user presses Enter, check the answer
        else if (event == Event::Return) {

            // Check if the user's input is correct, with fallback to the first translation if multiple translations are provided
            const bool correct = utils::string::is_answer_correct(user_input, current_entry.translation);

            // Insert the new history entry at the beginning of the history vector
            history.emplace(history.begin(), history_counter++, current_entry.kanji, current_entry.kana, current_entry.translation, current_entry.sentence_en, correct);

            // If the history vector is longer than 5 entries, remove the last entry to keep the history size at 5
            if (history.size() > 5) {
                history.pop_back();
            }

            // Clear the user's input
            user_input.clear();

            // Reset kana and answer to preferred user settings
            help_hint = HintState::Off;
            display_kana = args.display_kana;      // User preference
            display_answer = args.display_answer;  // User preference

            // Get a new random entry from the vocabulary
            current_entry = vocab->get_entry();
            return true;
        }
        // Otherwise, do nothing
        else {
            return false;
        }
    });

    // Main renderer for the main application screen
    const auto main_renderer = Renderer([&] {
                                   // If loading, render the loading screen
                                   if (is_loading) {
                                       // If an exception occurred, render the error screen
                                       if (load_exception) {
                                           try {
                                               std::rethrow_exception(load_exception);
                                           }
                                           catch (const std::exception &e) {
                                               return vbox({
                                                          // Title
                                                          text("将軍") | center | bold,
                                                          separator(),
                                                          // Error message, e.g., FileNotFoundError
                                                          vbox({
                                                              text(std::string(e.what())) | bold | center,
                                                          }) |
                                                              center | size(WIDTH, EQUAL, 90) | flex_grow | hcenter,
                                                      }) |
                                                      border | bgcolor(Color::Red) | center;
                                           }
                                       }
                                       // Otherwise, render the loading screen
                                       else {
                                           return loading_renderer->Render();
                                       }
                                   }

                                   // Otherwise, render the main application screen

                                   // Create UI elements for the history dynamically
                                   std::vector<Element> history_elements;
                                   history_elements.reserve(5);  // Reserve space for 5 elements
                                   for (const auto &entry : history) {
                                       history_elements.emplace_back(
                                           text(std::to_string(entry.number) + ". " + entry.kanji + "（" + entry.kana + "）" + "= " + entry.translation + " (" + entry.sentence_en + ")") |
                                           color(entry.is_correct ? Color::Green : Color::Red));
                                   }
                                   while (history_elements.size() < 5) {
                                       history_elements.emplace_back(text(" "));
                                   }

                                   const auto history_box = vbox(std::move(history_elements)) | border | size(WIDTH, EQUAL, 90) | center;

                                   return vbox({
                                              text("将軍") | center | bold,
                                              separator(),
                                              vbox({
                                                  text("漢字：" + current_entry.kanji + (display_kana ? "（" + current_entry.kana + "）" : "") + (display_answer ? "= " + current_entry.translation + "" : "")) | bold | size(WIDTH, EQUAL, 90),
                                                  text("例文：" + current_entry.sentence_jp) | bold | size(WIDTH, EQUAL, 90),
                                                  text("POS: " + current_entry.pos) | bold | size(WIDTH, EQUAL, 90),
                                                  input_with_enter->Render() | bold | bgcolor(Color::Magenta) | border,
                                              }) | border |
                                                  center | size(WIDTH, EQUAL, 90) | flex_grow | hcenter,
                                              separator(),
                                              text("履歴") | bold | center,
                                              history_box,
                                          }) |
                                          border | bgcolor(Color::Grey11) | center;
                               }) |
                               bgcolor(Color::Pink1) | color(Color::Pink1);

    // Run the loading thread
    std::thread load_thread([&]() {
        try {
            // Load JSON file from disk
            vocab = std::make_unique<io::kanji::Vocabulary>(core::filepaths::vocabulary);
            // Get random entry from the vocabulary
            current_entry = vocab->get_entry();
            is_loading = false;
        }
        // Capture any exception, e.g., FileNotFoundError
        catch (...) {
            load_exception = std::current_exception();
        }
        // Set loading flag to false and post a custom event to trigger the screen update

        screen.PostEvent(Event::Custom);
    });

    // Detach the thread to avoid blocking
    load_thread.detach();

    // Main loop with a container to handle focus
    const auto main_container = Container::Vertical({
        input_with_enter,
    });

    const auto main_app = Renderer(main_container, [&] {
        return main_renderer->Render();
    });

    screen.Loop(main_app);
}
