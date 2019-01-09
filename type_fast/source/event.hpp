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

#ifndef __EVENT_HPP__
#define __EVENT_HPP__

// ============================================================ //
// Headers
// ============================================================ //

#include "widget/constants.hpp"

// ============================================================ //
// Types
// ============================================================ //

namespace tf
{

enum class Event_type
{
    none,
    // Press enter in inputbox.
    word_input,
    // When a word scrolls off the screen.
    word_missed,
    // When you entered a word that exists.
    word_hit
};

struct Event
{
    Event_type type;
    void* data;

    // ============================================================ //
    // Lifetime
    // ============================================================ //
    Event() : type(Event_type::none), data(nullptr) {}
    Event(Event_type type, void* data) : type(type), data(data) {}

    // no copy because of destructor
    Event(const Event& other) = delete;
    Event& operator=(const Event& other) = delete;

    // move - needed for std::vector
    Event(Event&& other) noexcept : type(other.type), data(other.data)
        {
            other.data = nullptr;
        };
    Event& operator=(Event&& other) noexcept
        {
            if (this != &other) {
                type = other.type;
                data = other.data;
                other.data = nullptr;
            }
            return *this;
        }

    // RAII free our data
    ~Event() { delete data; }
};

struct Event_word_input
{
    static constexpr int size = constants::word_size;
    char word[size];
};

struct Event_word_missed
{
    static constexpr int size = constants::word_size;
    char word[size];
};

struct Event_word_hit
{
    static constexpr int size = constants::word_size;
    char word[size];
};

// ============================================================ //
// Helper functions
// ============================================================ //

Event_word_input* create_event_word_input(const char* word);

Event_word_missed* create_event_word_missed(const char* word);

Event_word_hit* create_event_word_hit(const char* word);


}

#endif//__EVENT_HPP__
