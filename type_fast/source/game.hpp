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
#include "util/view.hpp"

// ============================================================ //
// Class
// ============================================================ //

namespace tf
{
    /**
     * Mainly holds global state, also does some setup.
     */
    class Game
    {
    public:
        Game(int width, int height, const char* title, int target_fps);

        int width() const { return m_width; }
        int height() const { return m_height; }

        inline void set_inputbox(View<char> inputbox_view) { m_inputbox_view = inputbox_view; }
        inline View<char> inputbox() const { return m_inputbox_view; }

    private:
        View<char> m_inputbox_view{};
        int m_width;
        int m_height;
    };
}

#endif//__GAME_HPP__