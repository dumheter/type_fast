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

#include "game.hpp"

#include "thirdparty/dutil/stopwatch.hpp"

namespace tf
{

void Game::cleanup()
{
    // UnloadTexture(gnome);
    UnloadFont(m_font);
    CloseWindow();
}

void Game::run()
{
    while (!WindowShouldClose()) {
        update();
        draw();
    }
}

void Game::setup(int width, int height, const char* title, int target_fps,
                 const char* font, const char* text_file)
{
    m_width = width;
    m_height = height;
    InitWindow(width, height, title);
    SetTargetFPS(target_fps);
    m_font = LoadFontEx(font, 96, 224, NULL);
    SetTextureFilter(m_font.texture, FILTER_BILINEAR);

    load_word_generator(text_file);

    setup_start_objects();
}

void Game::setup_start_objects()
{
    // input box
    constexpr float inputbox_width = 300;
    m_input_box = tf::create_input_box(
        {{m_width/2-inputbox_width/2,600, inputbox_width, 40},
         tf::col_gray, tf::col_lightgray, 4},
        tf::col_white, tf::col_darkergray
        );

    // Reset button
    //auto reset_fn = [](decltype(hscroll_words)& map) { map.clear(); };
    tf::Rect reset_btn_rect{{10, 680, 100, 30}, tf::col_darkerblue,
                            tf::col_darkblue, 4};
    buttons.push_back(
        tf::create_button(
            reset_btn_rect, "reset", tf::col_lighterblue,
            [](){
                printf("cannot reset :(\n");
            })
        );

    // wpm
    {
        Rectangle wpm_slider_pos{10, (float)m_height - 100, 150, 50};
        auto wpm_slider = tf::create_slider(wpm_slider_pos, "wpm %d", tf::col_orange, 30,
                                            1, 170, true);
        sliders.push_back(std::move(wpm_slider));
    }
    m_wpm_view = &sliders.back().value;
    double wmp_timer = GetTime();

    // Title
    const float tfwidth = MeasureTextEx(m_font, "Type Fast", 50, 0).x;
    words.push_back({ "Type Fast", tf::col_red, 50, {m_width/2 - tfwidth/2, -5} });

    // Frametime
    word_formatters.push_back
        ({{"", tf::col_white, 20, {120, 0}},
          [](tf::Word_formatter* wf) {
              sprintf_s(wf->handle.text, wf->handle.text_size,
                        "Frametime %.2f ms", GetFrameTime());
          }
        });

    // Mouse pos
    word_formatters.push_back
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
}

void Game::update()
{
    if (IsKeyPressed(KEY_F11)) {
        ToggleFullscreen();
    }

    // Spawn new word
    if (GetTime() - m_wpm_timer > 60.0 / *m_wpm_view) {
        m_wpm_timer = GetTime();
        spawn_word();
    }

    const bool is_left_pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    const bool is_left_down = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    const Vector2 cpos = GetMousePosition();
    const int last_key = GetKeyPressed();
    const auto on_word_missed =
        []() {
            Game& game = Game::instance();
            constexpr float speed = 5.0f;
            constexpr float size = 50;
            tf::Rect rect{{game.width() - size, 0, size, (float)game.height()},
                          tf::col_white, {0xFF,0,0,100}, 0};
            bool found = false;
            for (auto& hscroll_rect : game.hscroll_rects) {
                if (!hscroll_rect.active) {
                    found = true;
                    hscroll_rect.active = true;
                    hscroll_rect.drawable = rect;
                }
            }
            if (!found) {
                game.hscroll_rects.push_back({
                        std::move(rect), speed, size, true,
                        [](tf::H_scroll<tf::Rect>* hscroll){
                            hscroll->active = false;}}
                    );
            }
        };

    tf::update(m_input_box, last_key, hscroll_words);
    for (auto& formatter : word_formatters) {
        tf::update(formatter);
    }
    for (auto& button : buttons) {
        tf::update(button, cpos, is_left_pressed);
    }
    for (auto it = hscroll_words.begin(); it != hscroll_words.end();) {
        if (tf::update(it->second, (float)m_width)) {
            it = hscroll_words.erase(it);
            on_word_missed();
        }
        else it++;
    }
    for (auto& hscroll_rect : hscroll_rects) {
        tf::update(hscroll_rect, (float)m_width);
    }
    for (auto& slider : sliders) {
        tf::update(slider, cpos, is_left_down);
    }
}

void Game::spawn_word()
{
    constexpr int top = 100;
    const int bot = m_height - 200;
    std::uniform_int_distribution<int> y_dist(top, bot);
    std::uniform_real_distribution<float> speed_dist(0.2f, 0.7f);
    std::uniform_int_distribution<int> x_dist(-200, -100);

    std::string str = m_wordgen.next();
    tf::Word word{"", tf::col_white, 30, {(float)x_dist(m_re), (float)y_dist(m_re)}};
    strcpy_s(word.text, word.text_size, str.c_str());
    H_scroll_hl_word obj = {
        {std::move(word), tf::col_green, 0,
         []() -> const char* {
             return Game::instance().m_input_box.text_input.text.text; }
        },
        (float)speed_dist(m_re), 0, true,
        [](tf::H_scroll<tf::Text_highlightable<tf::Word>>* hscroll) {
            hscroll->active = false;
         }
    };
    tf::H_scroll_set_width(&m_font, obj);
    hscroll_words.insert({std::move(str), std::move(obj)});
}


void Game::draw()
{
    BeginDrawing();
    {
        // ============================================================ //
        // Draw
        // ============================================================ //
        ClearBackground(tf::col_cornflower_blue);
        DrawFPS(5, 5);

        for (const auto& word : words) {
            tf::draw(&m_font, word);
        }
        for (const auto& text : texts) {
            tf::draw(&m_font, text);
        }
        for (const auto& formatter : word_formatters) {
            tf::draw(&m_font, formatter);
        }
        for (const auto& map : hscroll_words) {
            tf::draw(&m_font, map.second);
        }
        for (const auto& rect : rects) {
            tf::draw(&m_font, rect);
        }
        for (const auto& button : buttons) {
            tf::draw(&m_font, button);
        }
        for (const auto& slider : sliders) {
            tf::draw(&m_font, slider);
        }
        for (const auto& hscroll_rect : hscroll_rects) {
            tf::draw(&m_font, hscroll_rect);
        }
        tf::draw(&m_font, m_input_box);
        // DrawTextureV(gnome, {100,100}, RAYWHITE);
    }
    EndDrawing();
}

void Game::load_word_generator(const char* wordfile)
{
    dutil::Stopwatch sw{};
    sw.start();
    tf::File file{wordfile};
    if (file.has_error()) {
        printf("test has error %s.\n", file.error_to_string().c_str());
        assert(false && "Unable to open dict");
    }
    sw.stop();
    printf("load file: %.2f ms\n", sw.fnow_ms());
    printf("file size: %llu\n", file.size());

    sw.start();
    DelimSettings settings{ true, false, false, false, false };
    m_wordgen.load(file, settings);
    sw.stop();
    printf("init wordgen: %.2f ms\n", sw.fnow_ms());
    printf("Words loaded: %llu.\n", m_wordgen.word_count());

}

}
