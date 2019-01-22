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
#include "util/types.hpp"

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
    word_hit,
    // When you type your first letter into the inputbox.
    first_letter_input
};

// ============================================================ //
// Event data
// ============================================================ //

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

struct Event_first_letter_input
{
    char8 letter[4]; // UTF-8 letter
}; static_assert(sizeof(Event_first_letter_input) <= sizeof(void*),
                 "Event_first_letter_input must fit in void*");

// ============================================================ //
// Event
// ============================================================ //

class Event
{
public:
    // ============================================================ //
    // Factory functions
    // ============================================================ //
    /**
     * @param word Null terminated string
     */
    static Event create_word_input(const char* word);
    static Event create_word_missed(const char* word);
    static Event create_word_hit(const char* word);
    /**
     * Note, letters are encoded as UTF-8.
     * @param letter Pointer to where the letter begins
     * @param bytes How many bytes the letter is
     */
    static Event create_first_letter_input(const char* letter, const size_t bytes);

    // ============================================================ //
    // Getters
    // ============================================================ //
    const Event_word_input* get_word_input() const;
    const Event_word_missed* get_word_missed() const;
    const Event_word_hit* get_word_hit() const;
    const Event_first_letter_input get_first_letter_input() const;

    Event_type get_type() const { return this->type; };

    // ============================================================ //
    // Lifetime
    // ============================================================ //
private:
    Event() : type(Event_type::none), data(nullptr) {}
    Event(Event_type type, void* data) : type(type), data(data) {}

public:
    // no copy, because why
    Event(const Event& other) = delete;
    Event& operator=(const Event& other) = delete;

    Event(Event&& other) noexcept : type(other.type), data(other.data)
        {
            other.data = nullptr;
        };
    Event& operator=(Event&& other) noexcept
        {
            if (this != &other) {
                this->type = other.type;
                this->data = other.data;
                other.data = nullptr;
            }
            return *this;
        }

    // RAII free our data
    ~Event()
        {
            switch (this->type) {
            case Event_type::word_input: {
                delete (Event_word_input*)this->data;
                break;
            }
            case Event_type::word_missed: {
                delete (Event_word_missed*)this->data;
                break;
            }
            case Event_type::word_hit: {
                delete (Event_word_hit*)this->data;
                break;
            }
            case Event_type::first_letter_input:
            case Event_type::none: {
                break;
            }
            }
        }

    // ============================================================ //
    // Variables
    // ============================================================ //
private:
    Event_type type;
    void* data;
};

}

#endif//__EVENT_HPP__
