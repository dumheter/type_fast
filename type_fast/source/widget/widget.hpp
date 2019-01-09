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

#ifndef __WIDGET_HPP__
#define __WIDGET_HPP__

// ============================================================ //
// Headers
// ============================================================ //

#include <raylib.h>
#include <unordered_map>

// ============================================================ //
// Struct
// ============================================================ //

namespace tf
{

struct Word
{
    static constexpr int text_size = 40;
    char text[text_size];
    Color color;
    float font_size;
    Vector2 pos;
};

struct Text
{
    static constexpr int text_size = 256;
    char text[text_size];
    Color color;
    float font_size;
    Vector2 pos;
};

struct Word_formatter {
    Word handle;
    /**
     * You typically want to use this in your formatter function:
     * sprintf_s(wf->handle.text, wf->handle.text_size, "text %d", value);
     */
    void (*format_fn)(Word_formatter*);
};

/**
 *
 */
template <typename TText>
struct Text_highlightable {
    TText handle;
    Color highlight_color;
    int highlight_count;
    const char* (*get_highlight_count)();
};

/**
 * The drawable element will scroll horizontally with speed TODO.
 */
template <typename TDrawable>
struct H_scroll {
    TDrawable drawable;
    float speed;
    /**
     * If TDrawable is text, use the H_scroll_set_width helper function to set it.
     */
    float width;
    bool active;
    void (*on_out)(H_scroll<TDrawable>*);
};

/**
 * Rectangle.
 * To dissable outline, use @outline_thickness of 0.
 */
struct Rect {
    Rectangle rect;
    Color col_outline;
    Color col_bg;
    int outline_thickness;
};

/**
 * Button
 */
template <typename TText>
struct Button {
    TText text;
    Rect rect;
    Color col_bg;
    Color col_hover;
    void (*on_pressed)();
    bool hovering = false;
};

/**
 * Text input, will contain itself inside @max_width.
 */
template <typename TText>
struct Text_input {
    TText text;
    int text_pos;
    Color col_marker;
    float max_width;
    bool active;
};

template <typename TText_input>
struct Input_box {
    TText_input text_input;
    Rect rect;
};

// TODO
struct Checkbox {
    Word word;
    Rect rect;
    bool checked;
};

// slider helper, not drawable
struct LinePos {
    Vector2 a;
    Vector2 b;
};

struct Slider {
    Word title;
    char template_title[Word::text_size];
    int value;
    int min_value;
    int max_value;
    Color color;
    LinePos lmarker;
    LinePos rmarker;
    LinePos line;
    Rectangle marker;
    bool active;
    bool held;
    void (*on_change)(Slider&);
};

// ============================================================ //
// Functions
// ============================================================ //

Button<Word>
create_button(Rect rect, const char* word, Color word_color, void (*fn)());

Input_box<Text_input<Word>>
create_input_box(Rect rect, Color text_color, Color marker_color);

Checkbox create_checkbox(Vector2 pos, int size, const char* word, Color word_color,
                         bool checked = false);

/**
 * @pre Format must include the two letters %d, so it will format the value
 * correctly.
 */
Slider create_slider(Rectangle rect, const char* format, Color color,
                     void (*on_change)(Slider&), int default_val, int min_value = 0,
                     int max_value = 100, bool active = true);

// ============================================================ //

using H_scroll_hl_word = tf::H_scroll<tf::Text_highlightable<tf::Word>>;
using map_H_scroll = std::unordered_map<std::string, H_scroll_hl_word>;

void update(Word_formatter& word_formatter);
void update(Button<Word>& button, Vector2 mouse_pos, bool left_mouse_pressed);
void update(Input_box<Text_input<Word>>& input_box, int last_key, map_H_scroll& map);
void update(Text_highlightable<Word>& word_hl_scoll);

/**
 * @return If it left the screen
 */
bool update(H_scroll<Text_highlightable<Word>>& hscroll, float screen_width);
bool update(H_scroll<Rect>& hscroll, float screen_width);

void update(Slider& slider, Vector2 mouse_pos, bool left_mouse_down);

// ============================================================ //

void draw(Font* font, const Word& word);
void draw(Font* font, const Text& text);
void draw(Font* font, const Text_highlightable<Word>& hl_text);
void draw(Font* font, const Word_formatter& word_formatter);
template <typename TDrawable>
inline void draw(Font* font, const H_scroll<TDrawable>& TDrawable)
{
    if (TDrawable.active) {
        draw(font, TDrawable.drawable);
    }
}
void draw(Font* font, const Rect& rect);
void draw(Font* font, const Button<Word>& button);
void draw(Font* font, const Text_input<Word>& text_input);
void draw(Font* font, const Input_box<Text_input<Word>>& input_box);
void draw(Font* font, const Slider& slider);

// ============================================================ //

void H_scroll_set_width(Font* font, H_scroll<Text_highlightable<Word>>& hs);

void widget_debug_print_sizes();

}

#endif//__WIDGET_HPP__
