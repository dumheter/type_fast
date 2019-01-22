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

#ifndef __WPM_HPP__
#define __WPM_HPP__

// ============================================================ //
// Headers
// ============================================================ //

#include "widget.hpp"
#include "util/types.hpp"
#include <chrono>

// ============================================================ //
// Class
// ============================================================ //

namespace tf
{

class Wpm
{
public:
    Wpm() = default;

    /**
     * Call when a new word was entered and should be tracked.
     */
    void word_input(const size_t word_size);

    /**
     * Call when the first letter was entered. Used to detect when the player
     * goes afk.
     */
    void first_letter_input();

    /**
     * Call contineously to update the internal clocks.
     */
    void update();

    void reset();

    float get_wpm() const;

    /**
     * The adjusted length is supposed to normalize different words into a
     * standard lenght.
     */
    float get_adjusted_wpm() const;

private:
    u64 word_count = 0;
    u64 word_total_length = 0;
    static constexpr float word_adjusted_length = 4.0f;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_update{
        std::chrono::high_resolution_clock::now()
    };
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time{
        std::chrono::high_resolution_clock::now()
    };

    std::chrono::milliseconds active_time{0};
    std::chrono::milliseconds inactive_time{0};

    /**
     * We only start the wpm counter once the player has entered a letter.
     */
    bool has_begun = false;
};

}

#endif//__WPM_HPP__
