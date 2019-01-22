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

#include <cassert>
#include "thirdparty/dutil/stopwatch.hpp"

namespace tf
{

Game::~Game()
{
    // UnloadTexture(gnome);
    UnloadFont(m_font);
}

void Game::run()
{
    while (!WindowShouldClose()) {
        updatetime.start();
        update();
        updatetime.stop();
        updatetime_last = updatetime.fms();
        draw();
    }
}

void Game::setup(int width, int height, const char* title, int target_fps,
                 const char* font, const char* text_file)
{
    m_width = width;
    m_height = height;
    SetWindowSize(width, height);
    SetWindowTitle(title);
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
    constexpr float reset_btn_width = 100;
    tf::Rect reset_btn_rect{{m_width/2 - reset_btn_width/2, 680, reset_btn_width, 30},
                            tf::col_darkerblue,tf::col_darkblue, 4};
    buttons.push_back(
        tf::create_button(
            reset_btn_rect, "reset", tf::col_lighterblue,
            [](){
                Game::instance().reset_game();
            })
        );

    // wpm slider
    Rectangle wpm_slider_pos{10, (float)m_height - 100, 150, 50};
    constexpr int wpm_default = 40;
    sliders.push_back(
        tf::create_slider(
            wpm_slider_pos, "wpm %d", tf::col_orange,
            [](Slider& slider){
                Game& game = Game::instance();
                game.m_wpm = slider.value;
            },
            wpm_default, 1, 170, true)
        );

    // wpm counter
    Vector2 wpm_stats_pos{10, 30};
    word_formatters.push_back({
            {"", tf::col_white, 20, wpm_stats_pos},
            [](tf::Word_formatter* wf) {
                sprintf_s(wf->handle.text, wf->handle.text_size,
                          "wpm %.1f", Game::instance().get_wpm());
            }
        });
    word_formatters.push_back({
            {"", tf::col_white, 20, {wpm_stats_pos.x, wpm_stats_pos.y + 20}},
            [](tf::Word_formatter* wf) {
                sprintf_s(wf->handle.text, wf->handle.text_size,
                          "adjusted wpm %.1f", Game::instance().get_adjusted_wpm());
            }
        });

    // Title
    const float tfwidth = MeasureTextEx(m_font, "Type Fast", 50, 0).x;
    words.push_back({ "Type Fast", tf::col_red, 50, {m_width/2 - tfwidth/2, -5} });

    // Frametime
    constexpr float fps_x = 120;
    word_formatters.push_back
        ({{"", tf::col_white, 20, {fps_x, 0}},
          [](tf::Word_formatter* wf) {
              sprintf_s(wf->handle.text, wf->handle.text_size,
                        "Frametime %.2fms, Update %.2fms", GetFrameTime()*1000,
                        Game::instance().updatetime_last);
          }
        });
    const float fps_width = MeasureTextEx(
        m_font, "Frametime 12.73ms, Update 0.13ms", 20, 0
        ).x;

    // Mouse pos
    word_formatters.push_back
        ({{"", tf::col_white, 20, {fps_x + fps_width, 0}},
          [](tf::Word_formatter* wf) {
              sprintf_s(wf->handle.text, wf->handle.text_size,
                        "mouse %d:%d", GetMouseX(), GetMouseY());}
        });

    // Music
    music = Tfmusic{"res/audio/joy_of_painting.ogg"};
    music.play();

    // Volume slider
    Rectangle volume_slider_pos{10, (float)m_height-50, 150, 50};
    sliders.push_back(
        create_slider(
            volume_slider_pos, "Volume %d", tf::col_orange,
            [](Slider& slider) {
                Game& game = Game::instance();
                game.music.set_volume(slider.value / 100.0f);
            },
            60, 0, 100, true)
        );

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
    if (IsKeyPressed(KEY_F11)) { ToggleFullscreen(); }

    update_audio();
    update_game_objects();
    handle_events();
}

void Game::update_game_objects()
{
    // Spawn new word
    if (GetTime() - m_wpm_timer > 60.0 / m_wpm) {
        m_wpm_timer = GetTime();
        spawn_word();
    }

    const bool is_left_pressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    const bool is_left_down = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    const Vector2 cpos = GetMousePosition();

    tf::update(m_input_box, events);
    for (auto& formatter : word_formatters) {
        tf::update(formatter);
    }
    for (auto& button : buttons) {
        tf::update(button, cpos, is_left_pressed);
    }
    for (auto it = hscroll_words.begin(); it != hscroll_words.end();) {
        if (tf::update(it->second, (float)m_width)) {
            events.emplace_back(
                Event_type::word_missed,
                create_event_word_missed(it->second.drawable.handle.text)
                );
            it = hscroll_words.erase(it);
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

void Game::update_audio()
{
    music.update();
}

void Game::handle_events()
{
    for (const auto& event : events) {
        switch (event.type) {
        case Event_type::word_input: {
            on_word_input(event);
            break;
        }
        case Event_type::word_missed: {
            on_word_missed(event);
            break;
        }
        case Event_type::word_hit: {
            printf("word hit event not handled\n");
            break;
        }
        }
    }

    events.clear();
}

void Game::on_word_input(const Event& event)
{
    const auto len = strlen(static_cast<Event_word_input*>(event.data)->word);
    //TODO ugly string allocation
    std::string typed{static_cast<Event_word_input*>(event.data)->word, len};
    auto it = hscroll_words.find(typed);
    if (it != hscroll_words.end()) { // entered correct word
        hscroll_words.erase(it);
        m_wpm_stats.update(len);
    }
}

void Game::on_word_missed(const Event& event)
{
    constexpr float speed = 5;
    constexpr float size = 50;
    tf::Rect rect{{m_width - size, 0, size, (float)m_height},
                  tf::col_white, {0xFF,0,0,100}, 0};
    bool found = false;
    for (auto& hscroll_rect : hscroll_rects) {
        if (!hscroll_rect.active) {
            found = true;
            hscroll_rect.active = true;
            hscroll_rect.drawable = rect;
        }
    }
    if (!found) {
        hscroll_rects.push_back({
                std::move(rect), speed, size, true,
                [](tf::H_scroll<tf::Rect>* hscroll){
                    hscroll->active = false;}}
            );
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
    H_scroll<Text_highlightable<Word>> obj = {
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

void Game::reset_game()
{
    hscroll_words.clear();
}

}
