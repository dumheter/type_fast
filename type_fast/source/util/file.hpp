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

#ifndef __FILE_HPP__
#define __FILE_HPP__

// ============================================================ //
// Headers
// ============================================================ //

#include <string>
#include "thirdparty/filip/unicode.h"
#include <cassert>
#include <ostream>

// ============================================================ //
// Class
// ============================================================ //

namespace tf
{
enum class FileError : int
{
    UNKNOWN_ERROR = 0,
    NO_ERROR,
    CANNOT_OPEN_PATH,
    FAILED_TO_SEEK,
    FAILED_TO_READ,
    FAILED_TO_GET_POS
};

/**
 * TOOD use std::string instead of char*
 *
 * Read file from disk and store in buffer.
 *
 * Usage:
 * 1. Create the object with a valid path.
 * 2. Check that it does not has_error().
 * 3. Read from the buffer with get()
 */
class File
{
public:
    explicit File(const std::string& path);

    ~File();

    bool has_error() const { return this->error != FileError::NO_ERROR; }

    const char* get() const { return this->buf; }

    size_t get_size() const { return this->size; }

    std::string error_to_string() const;

    static bool file_exists(const std::string& path);

    /**
     * A single unicode code point encoded as utf8.
     *
     * Note, it will not allocate new memory, instead point to an
     * existing buffer. Thus you must decode the utf8 symbol to know
     * its size.
     */
    struct Utf8_symbol
    {
        char data[4];

        friend std::ostream& operator<<(std::ostream& os, const Utf8_symbol& cp) {
            u64 codepoint;
            u32 bytes;
            const bool res = lnUTF8Decode(reinterpret_cast<const char*>(cp.data), 0, &codepoint, &bytes);
            assert(res && "failed to decode utf8");

            for (u32 i=0; i<bytes; i++) {
                os << cp.data[i];
            }
            return os;
        }
    };



    // ============================================================ //
    // Iterator
    // ============================================================ //

    template <typename data_type>
    class utf8_iterator : public std::iterator<
        std::input_iterator_tag,   // iterator_category
        data_type,                 // value type
        ptrdiff_t,                 // difference_type
        data_type*,                // pointer
        data_type&                 // reference
        >
    {
    private:
        data_type* ptr;

    public:
        utf8_iterator(data_type* ptr = nullptr) : ptr(ptr) {}
        utf8_iterator(const utf8_iterator& other) : ptr(other.ptr) {}
        utf8_iterator& operator=(const utf8_iterator& other) { this->ptr = other.ptr; }
        utf8_iterator(utf8_iterator&& other) : ptr(other.ptr) { other.ptr = nullptr; }
        utf8_iterator& operator=(utf8_iterator&& other) {
            if (this != &other) {
                this->ptr = other.ptr;
                other.ptr = nullptr;
            }
            return *this;
        }

        operator bool() const { return this->ptr; }
        bool operator==(utf8_iterator other) const { return this->ptr == other.ptr; }
        bool operator!=(utf8_iterator other) const { return !(*this == other); }

        utf8_iterator& operator+=(const ptrdiff_t& diff) {
            this->ptr += diff;
            return *this;
        }
        utf8_iterator& operator-=(const ptrdiff_t& diff) {
            this->ptr -= diff;
            return *this;
        }
        utf8_iterator& operator++() {
            u64 cp;
            u32 bytes;
            const bool res = lnUTF8Decode(this->ptr, 0, &cp, &bytes);
            assert(res && "failed to decode utf8");
            this->ptr += bytes;
            return *this;
        }
        utf8_iterator& operator--() {
            this->ptr--;
            return *this;
        }
        utf8_iterator operator++(int) {
            auto tmp{*this};
            this->ptr++;
            return tmp;
        }
        utf8_iterator operator--(int) {
            auto tmp{*this};
            this->ptr--;
            return tmp;
        }
        utf8_iterator operator+(const ptrdiff_t& diff) {
            auto old_ptr = this->ptr;
            this->ptr += diff;
            auto tmp{*this};
            this->ptr = old_ptr;
            return tmp;
        }
        utf8_iterator operator-(const ptrdiff_t& diff) {
            auto old_ptr = this->ptr;
            this->ptr -= diff;
            auto tmp{*this};
            this->ptr = old_ptr;
            return tmp;
        }

        Utf8_symbol* operator*() { return reinterpret_cast<Utf8_symbol*>(this->ptr); }
        const Utf8_symbol* operator*() const { return reinterpret_cast<const Utf8_symbol*>(this->ptr); }
        Utf8_symbol* operator->() { return this->ptr; }

        data_type* get_ptr() const { return this->ptr; }
        const data_type* get_const_ptr() const { return this->ptr; }
    };

    using iterator = utf8_iterator<char>;
    using const_iterator = utf8_iterator<const char>;

    iterator begin() { return iterator(&this->buf[0]); }
    iterator end() { return iterator(&this->buf[this->size]); }

    const_iterator begin() const { return const_iterator(&this->buf[0]); }
    const_iterator end() const { return const_iterator(&this->buf[this->size]); }

private:
    FileError error = FileError::UNKNOWN_ERROR;
    char* buf = nullptr;
    size_t size = 0;
};

}

#endif//__FILE_HPP__
