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
    m_word_count++;
    m_word_total_length += word_size;
}

void Wpm::update()
{
    const std::chrono::time_point<std::chrono::high_resolution_clock> now =
        std::chrono::high_resolution_clock::now();
    m_active_time += std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_last_update);
    m_last_update = now;
}

void Wpm::reset()
{
    m_word_count = 0;
    m_word_total_length = 0;
    m_active_time = std::chrono::milliseconds(0);
    m_last_update = std::chrono::high_resolution_clock::now();
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
    return _wpm(m_active_time.count(), m_word_count);
}

float Wpm::get_adjusted_wpm() const
{
    return _wpm(m_active_time.count(),
                std::llround(m_word_total_length / m_word_adjusted_length));
}

}
