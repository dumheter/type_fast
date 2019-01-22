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

#include "widget.hpp"
#include "../util/util.hpp"
#include "../util/color.hpp"
#include "../util/assert.hpp"
#include "../thirdparty/filip/unicode.h"
#include <cstring>
#include <cstdlib>
#include <string>

namespace tf
{

constexpr float text_spacing = 0;

// ============================================================ //

Button<Word>
create_button(Rect rect, const char* word, Color word_color, void (*fn)())
{
    const float padding = rect.outline_thickness + 2.0f;
    const Vector2 word_pos{rect.rect.x + padding, rect.rect.y};
    const float font_size = rect.rect.height;
    constexpr unsigned char hover_tint = 30;
    const unsigned char r = static_cast<unsigned char>(clamp(rect.col_bg.r - hover_tint, 0, 255));
    const unsigned char g = static_cast<unsigned char>(clamp(rect.col_bg.g - hover_tint, 0, 255));
    const unsigned char b = static_cast<unsigned char>(clamp(rect.col_bg.b - hover_tint, 0, 255));
    const Color col_hover = {r, g, b, rect.col_bg.a};
    Button<Word> button{
        {"", word_color, font_size, word_pos}, rect, rect.col_bg, col_hover,
        fn, false
    };
    strcpy_s(button.text.text, button.text.text_size, word);
    return button;
}

Input_box<Text_input<Word>>
create_input_box(Rect rect, Color text_color, Color marker_color)
{
    const float padding = rect.outline_thickness + 2.0f;
    const Vector2 word_pos{rect.rect.x + padding, rect.rect.y};
    const float font_size = rect.rect.height;
    const float max_width = rect.rect.width - rect.outline_thickness*2;
    Input_box<Text_input<Word>> input_box{
        {{"", text_color, font_size, word_pos}, 0, marker_color, max_width, true},
        rect
    };
    return input_box;
}

constexpr float slider_height = 20;
constexpr float marker_width = slider_height * 2 / 5;
constexpr float marker_height = slider_height * 3 / 4;
static inline void slider_update_markerx(Slider& slider)
{
    const float delta = ((slider.line.b.x - slider.line.a.x) /
                         (float)(slider.max_value - slider.min_value));
    slider.marker.x = slider.lmarker.a.x + delta * slider.value - marker_width / 2;
}

static inline void slider_update_value(Slider& slider)
{
    const float delta = ((slider.line.b.x - slider.line.a.x) /
                         (float)(slider.max_value - slider.min_value));
    slider.value = lroundf(
        (slider.lmarker.a.x - slider.marker.x  - marker_width/2) / (-delta)
        );
}

Slider create_slider(Rectangle rect, const char* format, Color color,
                     void (*on_change)(Slider&), int default_val, int min_value,
                     int max_value, bool active)
{
    // NOTE more constants at slider_update_markerx()
    const float size = rect.height / 2;
    constexpr float linethic = 1;
    const float slidery = rect.y + size + slider_height / 2;
    constexpr float lmarker_height = 7;
    constexpr float rmarker_height = 13;
    LinePos lmarker{{rect.x, slidery - lmarker_height / 2},
                    {rect.x, slidery + lmarker_height / 2}};
    LinePos rmarker{{rect.x + rect.width, slidery - rmarker_height / 2},
                    {rect.x + rect.width, slidery + rmarker_height / 2}};
    LinePos line{{rect.x, slidery}, {rect.x + rect.width, slidery}};
    Rectangle marker{rect.x + rect.width / 2 - marker_width/2,
                     slidery - marker_height / 2, marker_width, marker_height};

    Word word{"", col_white, size, {rect.x, rect.y}};

    Slider slider{
        std::move(word), "", default_val, min_value, max_value, color, lmarker,
        rmarker, line, marker, active, false, on_change
    };
    slider_update_markerx(slider);

    auto ret = strcpy_s(slider.template_title, Word::text_size, format);
    constexpr int STRCPY_S_SUCCESS = 0;
    tf_assert(ret == STRCPY_S_SUCCESS, "failed to strcpy");

    const int sprintf_res = sprintf_s(slider.title.text, slider.title.text_size,
                                      slider.template_title, default_val);
    tf_assert(sprintf_res != 0, "sprintf error");

    slider.on_change(slider);

    return slider;
}

// ============================================================ //

void update(Word_formatter& formatter)
{
    formatter.format_fn(&formatter);
}

void update(Button<Word>& button, Vector2 mouse_pos, bool left_mouse_pressed)
{
    const auto x = button.rect.rect.x;
    const auto y = button.rect.rect.y;
    const bool cursor_inside =
        tf::inside(mouse_pos.x, x, x + button.rect.rect.width) &&
        tf::inside(mouse_pos.y, y, y + button.rect.rect.height);
    if (!button.hovering && cursor_inside) {
        button.hovering = true;
        button.rect.col_bg = button.col_hover;
    }
    else if (button.hovering && !cursor_inside) {
        button.hovering = false;
        button.rect.col_bg = button.col_bg;
    }
    if (cursor_inside && left_mouse_pressed) {
        button.on_pressed();
    }
}

void update(Input_box<Text_input<Word>>& input_box, std::vector<Event>& events)
{
    const int last_key = GetKeyPressed();

    if (input_box.text_input.active) {
        // alpah keypress
        if (last_key >= 33 && last_key <= 255 &&
            input_box.text_input.text_pos + 2 < input_box.text_input.text.text_size) {
            const int bytes = lnUTF8Encode(input_box.text_input.text.text +
                                           input_box.text_input.text_pos, last_key);
            input_box.text_input.text_pos += bytes;
        }
        // backspace
        else if (IsKeyPressed(KEY_BACKSPACE) && input_box.text_input.text_pos > 0) {
            const int bytes = lnUTF8Seek(input_box.text_input.text.text,
                                         input_box.text_input.text_pos);
            input_box.text_input.text_pos -= bytes;
            memset(input_box.text_input.text.text + input_box.text_input.text_pos,
                   0, bytes);
        }

        // space or enter
        else if ((IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER))
                 && input_box.text_input.text_pos > 0) {

            Event_word_input* event_data = create_event_word_input(
                input_box.text_input.text.text);
            events.emplace_back(Event_type::word_input, event_data);

            input_box_clear(input_box);
        }
    }
}

void update(Text_highlightable<Word>& hl_text)
{
    const char* texta = hl_text.get_highlight_count();
    const char* textb = hl_text.handle.text;
    int matching = 0;
    int pos = 0;
    while (pos < hl_text.handle.text_size) {

        u64 cpa, cpb;
        u32 bytesa, bytesb;
        const int valida = lnUTF8Decode(texta, pos, &cpa, &bytesa);
        const int validb = lnUTF8Decode(textb, pos, &cpb, &bytesb);

        if (valida + validb != 2 || cpa != cpb || bytesa != bytesb || cpa == 0) break;
        else {
            matching++;
            pos += bytesa;
        }
    }

    hl_text.highlight_count = matching;
}

bool update(H_scroll<Text_highlightable<Word>>& hscroll, float screen_width)
{
    if (hscroll.active) {
        hscroll.drawable.handle.pos.x += hscroll.speed;
        if (hscroll.drawable.handle.pos.x > screen_width) {
            hscroll.on_out(&hscroll);
            return true;
        }
        update(hscroll.drawable);
    }
    return false;
}

bool update(H_scroll<Rect>& hscroll, float screen_width)
{
    if (hscroll.active) {
        hscroll.drawable.rect.x += hscroll.speed;
        if (hscroll.drawable.rect.x > screen_width) {
            hscroll.on_out(&hscroll);
            return true;
        }
    }
    return false;
}

void update(Slider& slider, Vector2 mouse_pos, bool left_mouse_down)
{
    if (left_mouse_down && !slider.held && slider.active) {

        if (inside(mouse_pos.x, slider.marker.x,
                   slider.marker.x+slider.marker.width) &&
            inside(mouse_pos.y, slider.marker.y,
                   slider.marker.y+slider.marker.height)) {
            slider.held = true;
        }
    }

    if (slider.held) {
        if (!left_mouse_down) slider.held = false;
        else {
            const float newx = clamp(mouse_pos.x, slider.line.a.x, slider.line.b.x);
            slider.marker.x = newx - marker_width/2;
            const int oldvalue = slider.value;
            slider_update_value(slider);
            if (oldvalue != slider.value) {
                const int sprintf_res = sprintf_s(
                    slider.title.text, slider.title.text_size,
                    slider.template_title, slider.value);
                slider.on_change(slider);
                tf_assert(sprintf_res != 0, "sprintf error");
            }
        }

    }
}

// ============================================================ //

void draw(Font* font, const Word& word)
{
    DrawTextEx(*font, word.text, word.pos, word.font_size, text_spacing, word.color);
}

void draw(Font* font, const Text& text)
{
    DrawTextEx(*font, text.text, text.pos, text.font_size, text_spacing, text.color);
}

void draw(Font* font, const Text_highlightable<Word>& hl_text)
{
    const int hlcount = hl_text.highlight_count;
    if (hlcount == 0) draw(font, hl_text.handle);
    else {
        const auto textlen = lnUTF8StringLength(hl_text.handle.text);
        if (textlen != hlcount) {
            char* const text1 = lnUTF8Substring(hl_text.handle.text, 0, hlcount);
            DrawTextEx(*font, text1, hl_text.handle.pos, hl_text.handle.font_size,
                       text_spacing, hl_text.highlight_color);


            if (hlcount < textlen) {
                char* const text2 = lnUTF8SubstringFrom(hl_text.handle.text, hlcount);
                const Vector2 hltextlen = MeasureTextEx(*font, text1, hl_text.handle.font_size,
                                                        text_spacing);
                const Vector2 pos{hl_text.handle.pos.x+hltextlen.x, hl_text.handle.pos.y};
                DrawTextEx(*font, text2, pos, hl_text.handle.font_size,
                           text_spacing, hl_text.handle.color);
                free(text2);
            }
            free(text1);
        }
        else { // Only the highlighted color will be drawn, so dont make substring
            DrawTextEx(*font, hl_text.handle.text, hl_text.handle.pos, hl_text.handle.font_size,
                       text_spacing, hl_text.highlight_color);
        }
    }
}

void draw(Font* font, const Word_formatter& formatter)
{
    draw(font, formatter.handle);
}

void draw(Font* font, const Rect& rect)
{
    DrawRectangleRec(rect.rect, rect.col_bg);
    if (rect.outline_thickness > 0) {
        DrawRectangleLinesEx(rect.rect, rect.outline_thickness, rect.col_outline);
    }
}

void draw(Font* font, const Button<Word>& button)
{
    draw(font, button.rect);
    draw(font, button.text);
}

void draw(Font* font, const Text_input<Word>& text_input)
{
    // text
    float width = 0;
    if (text_input.text_pos > 0) {
        char* text = lnUTF8Substring(text_input.text.text, 0, text_input.text_pos);
        tf_assert(text, "Failed to create substring");
        DrawTextEx(*font, text, text_input.text.pos, text_input.text.font_size,
                   text_spacing, text_input.text.color);
        if (text_input.active) {
            width = MeasureTextEx(*font, text, text_input.text.font_size, text_spacing).x;
        }
        free(text);
    }

    // marker
    if (text_input.active) {
        constexpr float height = 6;
        const Rectangle rect{
            text_input.text.pos.x + width,
            text_input.text.pos.y + text_input.text.font_size - height*2,
            text_input.text.font_size/2.0f,
            height};
        DrawRectangleRec(rect, text_input.col_marker);
    }
}

void draw(Font* font, const Input_box<Text_input<Word>>& input_box)
{
    draw(font, input_box.rect);
    draw(font, input_box.text_input);
}

void draw(Font* font, const Slider& slider)
{
    draw(font, slider.title);
    DrawLineV(slider.lmarker.a, slider.lmarker.b, col_darkerblue);
    DrawLineV(slider.rmarker.a, slider.rmarker.b, col_darkerblue);
    DrawLineV(slider.line.a, slider.line.b, col_darkerblue);
    DrawRectangleRec(slider.marker, slider.color);
}

// ============================================================ //

void H_scroll_set_width(Font* font, H_scroll<Text_highlightable<Word>>& hs)
{
    const Vector2 len = MeasureTextEx(
        *font,
        hs.drawable.handle.text,
        hs.drawable.handle.font_size, text_spacing);
    hs.width = len.x;
}

void input_box_clear(Input_box<Text_input<Word>>& input_box)
{
    const auto len = strlen(input_box.text_input.text.text);
    memset(input_box.text_input.text.text, 0, len);
    input_box.text_input.text_pos = 0;
}

void widget_debug_print_sizes()
{
    printf("=== widget_debug_print_sizes\n");
    printf("= Word\t%zi\n", sizeof(Word));
    printf("= Text\t%zi\n", sizeof(Text));
    printf("= Word_formatter<Word> /wo Word\t%zi\n", sizeof(Word_formatter) - sizeof(Word));
    printf("= Word_formatter<Text> /wo Text\t%zi\n", sizeof(Word_formatter) - sizeof(Text));
    printf("= Text_highlightable<T>\t%zi\n", sizeof(Text_highlightable<Word>) -
           sizeof(Word));
    printf("= H_scroll<T>\t%zi\n", sizeof(H_scroll<Word>) - sizeof(Word));
    printf("= Rect\t%zi\n", sizeof(Rect));
    printf("= Button<T>\t%zi\n", sizeof(Button<Word>) - sizeof(Word));
    printf("= Text_input<T>\t%zi\n", sizeof(Text_input<Word>) - sizeof(Word));
    printf("= Input_box<T>\t%zi\n", sizeof(Input_box<Word>) - sizeof(Word));
    printf("===\n");
}

}
