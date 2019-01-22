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

#include "wpm.hpp"
#include <cmath>

namespace tf
{


void Wpm::word_input(const size_t word_size)
{
    this->last_update = std::chrono::high_resolution_clock::now();
    this->word_count++;
    this->word_total_length += word_size;
}

void Wpm::first_letter_input()
{
    if (!this->has_begun)
        this->inactive_time = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - start_time);
    this->has_begun = true;
}

void Wpm::update()
{
    this->active_time = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::high_resolution_clock::now() - start_time - inactive_time);
}

void Wpm::reset()
{
    this->word_count = 0;
    this->word_total_length = 0;
    this->last_update = std::chrono::high_resolution_clock::now();
    this->start_time = std::chrono::high_resolution_clock::now();
    this->active_time = std::chrono::milliseconds(0);
    this->inactive_time = std::chrono::milliseconds(0);
    this->has_begun = false;
}

static inline float _wpm(u64 active_time, u64 word_count)
{
    const float ms_per_word =
        static_cast<float>(active_time) / word_count;
    constexpr float ms_per_minute = 60 * 1000;

    return ms_per_minute / ms_per_word;
}

float Wpm::get_wpm() const
{
    return (this->word_count > 0 ?
        _wpm(this->active_time.count(), this->word_count) : 0.0f);
}

float Wpm::get_adjusted_wpm() const
{
    return (this->word_count > 0 ?
            _wpm(this->active_time.count(), std::llround(
                     this->word_total_length / this->word_adjusted_length)) : 0.0f);
}

}
