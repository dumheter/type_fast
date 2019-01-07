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
#include "util/win.hpp"
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
#include "game.hpp"

// ============================================================ //
// Globals
// ============================================================ //

constexpr int swidth = 1280;
constexpr int sheight = 720;
tf::Game game{swidth, sheight, "Type Fast", 144};

// ============================================================ //
// Main
// ============================================================ //

int main(int, char**)
{
    tf::fix_console(); // make it use UTF8

    // ============================================================ //
    // setup
    // ============================================================ //
    auto font_open_sans = LoadFontEx("res/fonts/open-sans/OpenSans-Regular.ttf",
                                     96, 224, NULL);
    SetTextureFilter(font_open_sans.texture, FILTER_BILINEAR);

    // ============================================================ //
    // Load dictionary
    // ============================================================ //
    dutil::Stopwatch sw{};
    sw.start();
    tf::File file{"res/dict/mobydick.txt"};
    if (file.has_error()) {
        printf("test has error %s.\n", file.error_to_string().c_str());
        assert(false && "Unable to open dict");
    }
    sw.stop();
    printf("load file: %.2f ms\n", sw.fnow_ms());
    printf("file size: %llu\n", file.size());

    sw.start();
    DelimSettings settings{ true, false, false, false, false };
    tf::Word_generator wordgen{file, settings};
    sw.stop();
    printf("init wordgen: %.2f ms\n", sw.fnow_ms());
    printf("Words loaded: %llu.\n", wordgen.word_count());

    // ============================================================ //
    // Declare our Data Driven containers
    // ============================================================ //
    std::vector<tf::Word> buf_word;
    std::vector<tf::Text> buf_text;
    std::vector<tf::Word_formatter> buf_word_formatter;
    using H_scroll_hl_word = tf::H_scroll<tf::Text_highlightable<tf::Word>>;
    std::unordered_map<std::string, H_scroll_hl_word> map_h_scroll;
    std::vector<tf::Rect> buf_rect;
    std::vector<tf::Button<tf::Word>> buf_button;
    std::vector<tf::Input_box<tf::Text_input<tf::Word>>> buf_input_box;
    std::vector<tf::Slider> buf_slider;
    std::vector<tf::H_scroll<tf::Rect>> buf_hscroll_rect;

    // ============================================================ //
    // Game Objects
    // ============================================================ //

    // input box
    constexpr float width = 300;
    buf_input_box.push_back
        (tf::create_input_box({{game.width()/2-width/2,600, width, 40}, tf::col_gray, tf::col_lightgray, 4},
                              tf::col_white, tf::col_darkergray)
         );
    auto& the_inputbox = buf_input_box.back();
    game.set_inputbox({the_inputbox.text_input.text.text,
                       the_inputbox.text_input.text.text +
                       the_inputbox.text_input.text.text_size});

    // Reset button
    //auto reset_fn = [](decltype(map_h_scroll)& map) { map.clear(); };
    tf::Rect reset_btn_rect{{10, 680, 100, 30}, tf::col_darkerblue, tf::col_darkblue, 4};
    buf_button.push_back
        (tf::create_button(reset_btn_rect, "reset", tf::col_lighterblue,
                           [](){printf("cannot reset :(\n");}));

    // generate the words
    std::random_device rd;
    std::default_random_engine re(rd());
    constexpr int top = 100;
    const int bot = game.height() - 200;
    std::uniform_int_distribution<int> y_dist(top, bot);
    std::uniform_real_distribution<float> speed_dist(0.2f, 0.7f);
    std::uniform_int_distribution<int> x_dist(-200, -100);

    // wpm
    {
        Rectangle wpm_slider_pos{10, (float)game.height() - 100, 150, 50};
        auto wpm_slider = tf::create_slider(wpm_slider_pos, "wpm %d", tf::col_orange, 30,
                                            1, 170, true);
        buf_slider.push_back(std::move(wpm_slider));
    }
    const int* wpm_view = &buf_slider.back().value;
    double wmp_timer = GetTime();

    // Title
    const float tfwidth = MeasureTextEx(font_open_sans, "Type Fast", 50, 0).x;
    buf_word.push_back({ "Type Fast", tf::col_red, 50, {game.width()/2 - tfwidth/2, -5} });

    // Frametime
    buf_word_formatter.push_back
        ({{"", tf::col_white, 20, {120, 0}},
          [](tf::Word_formatter* wf) {
              sprintf_s(wf->handle.text, wf->handle.text_size,
                        "Frametime %.2f ms", GetFrameTime());
          }
        });

    // Mouse pos
    buf_word_formatter.push_back
        ({{"", tf::col_white, 20, {250, 0}},
          [](tf::Word_formatter* wf) {
              sprintf_s(wf->handle.text, wf->handle.text_size,
                        "mouse %d:%d", GetMouseX(), GetMouseY());}
        });

    // ============================================================ //
    // Dummy objects
    // ============================================================ //

    // Texture2D gnome = LoadTexture("res/img/gnome_point.png");
    // Image gnome_img = GetTextureData(gnome);
    // const Vector2 gnome_info = {gnome_img.width, gnome_img.height};
    // UnloadImage(gnome_img);

    // ============================================================ //

    while (!WindowShouldClose()) {

        // ============================================================ //
        // Update
        // ============================================================ //
        if (IsKeyPressed(KEY_F11)) {
            ToggleFullscreen();
        }

        // Spawn new word
        if (GetTime() - wmp_timer > 60.0 / *wpm_view) {
            wmp_timer = GetTime();
            std::string str = wordgen.next();
            tf::Word word{"", tf::col_white, 30, {(float)x_dist(re), (float)y_dist(re)}};
            strcpy_s(word.text, word.text_size, str.c_str());
            H_scroll_hl_word obj =
                {{std::move(word), tf::col_green, 0, [](){ return game.inputbox().begin; }},
                 (float)speed_dist(re), 0, true,
                 [](tf::H_scroll<tf::Text_highlightable<tf::Word>>* hscroll) {
                     hscroll->active = false;
                 }
                };
            tf::H_scroll_set_width(&font_open_sans, obj);
            map_h_scroll.insert({std::move(str), std::move(obj)});
        }

        const bool is_left_pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
        const bool is_left_down = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
        const Vector2 cpos = GetMousePosition();
        const int last_key = GetKeyPressed();
        const auto on_word_missed =
            [&buf_hscroll_rect]() {
                constexpr float speed = 5.0f;
                constexpr float size = 50;
                tf::Rect rect{{game.width() - size, 0, size, (float)game.height()},
                              tf::col_white, {0xFF,0,0,100}, 0};
                bool found = false;
                for (auto& hscroll_rect : buf_hscroll_rect) {
                    if (!hscroll_rect.active) {
                        found = true;
                        hscroll_rect.active = true;
                        hscroll_rect.drawable = rect;
                    }
                }
                if (!found) {
                    buf_hscroll_rect.push_back({std::move(rect), speed, size, true,
                                                [](tf::H_scroll<tf::Rect>* hscroll){
                                                    hscroll->active = false;
                                                }});
                }
            };

        for (auto& formatter : buf_word_formatter) {
            tf::update(formatter);
        }
        for (auto& button : buf_button) {
            tf::update(button, cpos, is_left_pressed);
        }
        for (auto& input_box : buf_input_box) {
            tf::update(input_box, last_key, map_h_scroll);
        }
        for (auto it = map_h_scroll.begin(); it != map_h_scroll.end();) {
            if (tf::update(it->second, (float)game.width())) {
                it = map_h_scroll.erase(it);
                on_word_missed();
            }
            else it++;
        }
        for (auto& hscroll_rect : buf_hscroll_rect) {
            tf::update(hscroll_rect, (float)game.width());
        }
        for (auto& slider : buf_slider) {
            tf::update(slider, cpos, is_left_down);
        }

        BeginDrawing();
        {
            // ============================================================ //
            // Draw
            // ============================================================ //
            ClearBackground(tf::col_cornflower_blue);
            DrawFPS(5, 5);

            for (const auto& word : buf_word) {
                tf::draw(&font_open_sans, word);
            }
            for (const auto& text : buf_text) {
                tf::draw(&font_open_sans, text);
            }
            for (const auto& formatter : buf_word_formatter) {
                tf::draw(&font_open_sans, formatter);
            }
            for (const auto& map : map_h_scroll) {
                tf::draw(&font_open_sans, map.second);
            }
            for (const auto& rect : buf_rect) {
                tf::draw(&font_open_sans, rect);
            }
            for (const auto& button : buf_button) {
                tf::draw(&font_open_sans, button);
            }
            for (const auto& input_box : buf_input_box) {
                tf::draw(&font_open_sans, input_box);
            }
            for (const auto& slider : buf_slider) {
                tf::draw(&font_open_sans, slider);
            }
            for (const auto& hscroll_rect : buf_hscroll_rect) {
                tf::draw(&font_open_sans, hscroll_rect);
            }
            // DrawTextureV(gnome, {100,100}, RAYWHITE);
        }
        EndDrawing();
    }

    // UnloadTexture(gnome);
    UnloadFont(font_open_sans);

    CloseWindow();
    return 0;
}
