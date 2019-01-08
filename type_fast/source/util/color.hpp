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

#ifndef __COLOR_HPP__
#define __COLOR_HPP__

// ============================================================ //
// Headers
// ============================================================ //

#include <raylib.h>

// ============================================================ //
// Class
// ============================================================ //

namespace tf
{
/**
 * Complimentary colors to CORNFLOWER BLUE
 * 6495ED
 */
constexpr Color col_green{163, 204, 66, 255};
constexpr Color col_darkgreen{146, 178, 66, 255};
constexpr Color col_red{255, 150, 133, 255};
constexpr Color col_yellow{0xFF, 0xC6, 0x5C, 255};
constexpr Color col_lightyellow{0xFF, 0xD4, 0X85, 255};
constexpr Color col_lighteryellow{0xFF, 0xE7, 0xBA, 255};
constexpr Color col_darkyellow{0xFF, 0xB9, 0x36, 255};
constexpr Color col_darkeryellow{0xFF, 0xAA, 0x0E, 255};
constexpr Color col_lightgray{169, 169, 169, 255};
constexpr Color col_lightergray{209, 209, 209, 255};
constexpr Color col_gray{127, 127, 127, 255};
constexpr Color col_darkgray{66, 66, 66, 255};
constexpr Color col_darkergray{33, 33, 33, 255};
constexpr Color col_black{0, 0, 0, 255};
constexpr Color col_white{255, 255, 255, 255};

constexpr Color col_cornflower_blue{100, 149, 237, 255};
constexpr Color col_lightblue{0x8B, 0xB0, 0xF4, 255};
constexpr Color col_lighterblue{0xBD, 0xD3, 0xFB, 255};
constexpr Color col_darkblue{0x42, 0x7A, 0xE3, 255};
constexpr Color col_darkerblue{0x1F, 0x5E, 0xD2, 255};
constexpr Color col_purple{0x83, 0x68, 0xEE, 255};
constexpr Color col_teal{0x54, 0xEB, 0xDB, 255};
constexpr Color col_orange{0xFF, 0xA1, 0x5C, 255};

// copypaste template
//constexpr Color col_{, , , 255};
}

#endif//__COLOR_HPP__
