/**
 * MIT License
 *
 * Copyright (c) 2018 Christoffer Gustafsson
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __GAME_HPP__
#define __GAME_HPP__

// ============================================================ //
// Headers
// ============================================================ //

#include <raylib.h>
#include <vector>
#include <random>
#include <unordered_map>
#include "event.hpp"
#include "audio/tfmusic.hpp"
#include "audio/tfsound.hpp"
#include "thirdparty/filip/unicode.h"
#include "thirdparty/dutil/stopwatch.hpp"
#include "util/color.hpp"
#include "util/util.hpp"
#include "util/file.hpp"
#include "util/word_generator.hpp"
#include "util/raylib_lifetime.hpp"
#include "widget/wpm.hpp"
#include "widget/widget.hpp"
#include "widget/inputbox.hpp"

// ============================================================ //
// Class
// ============================================================ //

namespace tf
{

class Game
{
    // ============================================================ //
    // OpenGL context lifetime
    // ============================================================ //
    // must be first
    Raylib_lifetime raylib_lifetime{};

private:

    // ============================================================ //
    // Singleton
    // ============================================================ //
private:
    Game() = default;

public:
    static Game& instance() { static Game game{}; return game; }

    Game(const Game& other) = delete;
    Game& operator=(const Game& other) = delete;

    ~Game();

    /**
     * Main loop.
     */
    void run();

    // ============================================================ //
    // Setup
    // ============================================================ //
    /**
     * Call once before doing anything else.
     */
    void setup(int width, int height, const char* title, int target_fps,
                      const char* font, const char* text_file);

    void setup_start_objects();

    // ============================================================ //
    // Update
    // ============================================================ //
    void update();

    void update_game_objects();

    void update_audio();

    /**
     * Unhandled events will be destroyed.
     */
    void handle_events();

    void on_word_input(const Event& event);

    void on_word_missed(const Event& event);

    void spawn_word();

    // ============================================================ //
    // Draw
    // ============================================================ //
    void draw();

    // ============================================================ //
    // Modifiers
    // ============================================================ //
    void load_word_generator(const char* wordfile);

    void reset_game();

    // ============================================================ //
    // Lookup
    // ============================================================ //
    int width() const { return instance().m_width; }
    int height() const { return instance().m_height; }

    float get_wpm() const { return instance().m_wpm_stats.get_wpm(); }
    float get_adjusted_wpm() const {return instance().m_wpm_stats.get_adjusted_wpm();}

    // ============================================================ //
    // Misc
    // ============================================================ //


    // ============================================================ //
    // Private
    // ============================================================ //
private:
    int m_width = 0;
    int m_height = 0;
    Font m_font;
    Word_generator m_wordgen{};
    std::random_device m_rd{};
    std::default_random_engine m_re{m_rd()};

    // how fast words will be created
    double m_wpm_timer = 0;
    int m_wpm = 0;

    // track time spent in update
    dutil::Stopwatch updatetime{};
    double updatetime_last = 0;

    // track the players wpm
    Wpm m_wpm_stats{};

    // ============================================================ //
    // Game Objects
    // ============================================================ //
    Input_box<Text_input<Word>> m_input_box;
    std::vector<Word> words;
    std::vector<Text> texts;
    std::vector<Word_formatter> word_formatters;
    std::unordered_map<std::string, H_scroll<Text_highlightable<Word>>> hscroll_words;
    std::vector<Rect> rects;
    std::vector<Button<Word>> buttons;
    std::vector<Slider> sliders;
    std::vector<H_scroll<Rect>> hscroll_rects;
    Tfmusic music{};
    std::vector<Tfsound> sounds;
    std::vector<Event> events;
};

}

#endif//__GAME_HPP__
