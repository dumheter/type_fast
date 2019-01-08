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

#ifndef __INPUTBOX_HPP__
#define __INPUTBOX_HPP__

// ============================================================ //
// Headers
// ============================================================ //

#include <raylib.h>

// ============================================================ //
// Class
// ============================================================ //

namespace tf
{

enum class Inputbox_style
{
    standard,
    minimal
};

class Inputbox
{
public:
    Inputbox(Rectangle rect, Inputbox_style style, Font* font);

    void update(int key);

    void draw();

    char* get_text() const { return m_text; }

private:
    static constexpr int m_text_size = 256;
    int m_text_pos = 0;
    char* m_text;
    Inputbox_style m_style;
    Rectangle m_rect;
    Font* m_font;
};

}

#endif//__INPUTBOX_HPP__
