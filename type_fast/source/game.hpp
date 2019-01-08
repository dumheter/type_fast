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
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cassert>
#include <random>
#include <functional>
#include "util/color.hpp"
#include "util/util.hpp"
#include "util/file.hpp"
#include "util/dict.hpp"
#include "util/view.hpp"
#include "util/word_generator.hpp"
#include "widget/widget.hpp"
#include "widget/inputbox.hpp"
#include "thirdparty/filip/unicode.h"
#include "thirdparty/dutil/stopwatch.hpp"

// ============================================================ //
// Class
// ============================================================ //

namespace tf
{

enum class Event
{
    // Press enter in inputbox.
    text_input,
    // When a word scrolls off the screen.
    word_death
};

class Game
{
    // ============================================================ //
    // Singleton
    // ============================================================ //
private:
    Game() = default;

public:
    static Game& instance() { static Game game{}; return game; }

    Game(const Game& other) = delete;
    Game& operator=(const Game& other) = delete;

    // ============================================================ //
    // Must be called
    // ============================================================ //
    /**
     * Call before closing the program.
     */
    void cleanup();

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

    void spawn_word();

    // ============================================================ //
    // Draw
    // ============================================================ //
    void draw();

    // ============================================================ //
    // Modifiers
    // ============================================================ //
    void load_word_generator(const char* wordfile);

    // ============================================================ //
    // Lookup
    // ============================================================ //
    int width() const { return instance().m_width; }
    int height() const { return instance().m_height; }

    // ============================================================ //
    // Misc
    // ============================================================ //


    // ============================================================ //
    // Private
    // ============================================================ //
private:
    int m_width;
    int m_height;
    Font m_font;
    Word_generator m_wordgen{};
    std::random_device m_rd{};
    std::default_random_engine m_re{m_rd()};
    double m_wpm_timer;
    const int* m_wpm_view;

    // ============================================================ //
    // Game Objects
    // ============================================================ //
    tf::Input_box<tf::Text_input<tf::Word>> m_input_box;

    std::vector<tf::Word> words;
    std::vector<tf::Text> texts;
    std::vector<tf::Word_formatter> word_formatters;
    using H_scroll_hl_word = tf::H_scroll<tf::Text_highlightable<tf::Word>>;
    std::unordered_map<std::string, H_scroll_hl_word> hscroll_words;
    std::vector<tf::Rect> rects;
    std::vector<tf::Button<tf::Word>> buttons;
    std::vector<tf::Slider> sliders;
    std::vector<tf::H_scroll<tf::Rect>> hscroll_rects;
};

}

#endif//__GAME_HPP__
