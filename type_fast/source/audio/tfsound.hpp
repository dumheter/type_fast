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

#ifndef __TFSOUND_HPP__
#define __TFSOUND_HPP__

// ============================================================ //
// Headers
// ============================================================ //

#include <raylib.h>
#include <string>

// ============================================================ //
// Class
// ============================================================ //

namespace tf
{

class Tfsound
{
public:

    // ============================================================ //
    // Lifetime
    // ============================================================ //

    explicit Tfsound(const char* sound_path);

    Tfsound(const Tfsound& other) = delete;
    Tfsound& operator=(const Tfsound& other) = delete;

    ~Tfsound();

public:

    // ============================================================ //
    // Actions
    // ============================================================ //

    void play() const;

    void pause() const;

    void resume() const;

    void stop() const;

    // ============================================================ //
    // Info
    // ============================================================ //

    bool is_playing() const;

    std::string& get_name() { return m_name; }


private:
    Sound m_sound;
    std::string m_name;
};

}

#endif//__TFSOUND_HPP__
