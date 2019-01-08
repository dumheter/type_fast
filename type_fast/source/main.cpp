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

#include "game.hpp"
#include "util/win.hpp"

// ============================================================ //
// Main
// ============================================================ //

int main(int, char**)
{
    tf::fix_console(); // make it use UTF8

    constexpr int width = 1280;
    constexpr int height = 720;
    constexpr int target_fps = 144;
    const char* font = "res/fonts/open-sans/OpenSans-Regular.ttf";
    const char* text_file = "res/dict/mobydick.txt";
    tf::Game& game = tf::Game::instance();
    game.setup(width, height, "Type Fast", target_fps, font, text_file);
    game.run();
    game.cleanup();

    return 0;
}
