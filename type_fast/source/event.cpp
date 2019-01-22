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

#include "event.hpp"

#include <string.h>
#include "util/assert.hpp"
#include "util/types.hpp"
#include <cstring>

namespace tf
{

Event Event::create_word_input(const char* word)
{
    Event_word_input* event_data = new Event_word_input;
    const auto res = strcpy_s(event_data->word, event_data->size, word);
    tf_assert(res == 0, "copy word to event");

    Event event{Event_type::word_input, event_data};
    return event;
}

Event Event::create_word_missed(const char* word)
{
    Event_word_missed* event_data = new Event_word_missed;
    const auto res = strcpy_s(event_data->word, event_data->size, word);
    tf_assert(res == 0, "copy word to event");

    Event event{Event_type::word_missed, event_data};
    return event;
}

Event Event::create_word_hit(const char* word)
{
    Event_word_hit* event_data = new Event_word_hit;
    const auto res = strcpy_s(event_data->word, event_data->size, word);
    tf_assert(res == 0, "copy word to event");

    Event event{Event_type::word_hit, event_data};
    return event;
}

Event Event::create_first_letter_input(const char* letter, const size_t bytes)
{
    tf_assert(bytes <= sizeof(Event_first_letter_input),
              "could not fit letter in Event_first_letter_input struct");
    Event event{Event_type::first_letter_input, nullptr};
    std::memcpy(&event.data, letter, bytes);
    return event;
}

const Event_word_input* Event::get_word_input() const
{
    return static_cast<Event_word_input*>(this->data);
}

const Event_word_missed* Event::get_word_missed() const
{
    return static_cast<Event_word_missed*>(this->data);
}

const Event_word_hit* Event::get_word_hit() const
{
    return static_cast<Event_word_hit*>(this->data);
}

const Event_first_letter_input Event::get_first_letter_input() const
{
    Event_first_letter_input data{};
    std::memcpy(&data, &this->data, sizeof(Event_first_letter_input));
    return data;
}

}
