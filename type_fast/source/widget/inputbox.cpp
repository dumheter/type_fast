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

#include "inputbox.hpp"
#include <cstring>

Inputbox::Inputbox(Rectangle rect, Inputbox_style style, Font* font)
  : m_text(new char[m_text_size]), m_style(style), m_rect(rect), m_font(font)
{
  memset(m_text, 0, m_text_size);
}

void Inputbox::update(int key)
{
  if (key >= 32 && key <=255 && m_text_pos < m_text_size) {
    m_text[m_text_pos++] = static_cast<char>(key);
  }
  else if (IsKeyPressed(KEY_BACKSPACE) && m_text_pos > 0) {
    m_text[--m_text_pos] = '\0';
  }
}

void Inputbox::draw()
{
  if (m_style == Inputbox_style::standard) {
    constexpr Color color_dark_gray{33, 33, 33, 255};
    constexpr Color color_gray{99, 99, 99, 255};
    DrawRectangleRec(m_rect, color_gray);
    DrawRectangleLinesEx(m_rect, 4, color_dark_gray);
  }

  constexpr int padding = 8;
  constexpr float spacing = 0;
  const float fontsize = m_rect.height - padding*2;
  const Vector2 pos{m_rect.x + padding, m_rect.y + padding};
  DrawTextEx(*m_font, m_text, pos, fontsize, spacing, RAYWHITE);
  const Vector2 measure = MeasureTextEx(*m_font, m_text, fontsize, spacing);
  const Rectangle marker_rect{pos.x + measure.x, pos.y+measure.y - padding / 2,
                              fontsize/2, padding / 2};
  constexpr Color color_marker{240, 240, 0, 255};
  DrawRectangleRec(marker_rect, color_marker);
}
