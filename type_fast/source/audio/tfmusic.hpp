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

#ifndef __TFMUSIC_HPP__
#define __TFMUSIC_HPP__

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

/**
 * Music is an audio stream, thus update must be called continuously to
 * keep the audio buffer filled.
 */
class Tfmusic
{
public:

    // ============================================================ //
    // Lifetime
    // ============================================================ //

    /**
     * Only use this constructor to move another Tfmusic into it.
     */
    Tfmusic() = default;

    explicit Tfmusic(const char* music_path);
	explicit Tfmusic(const std::string& music_path) : Tfmusic(music_path.c_str()) {}

    Tfmusic(const Tfmusic& other) = delete;
    Tfmusic& operator=(const Tfmusic& other) = delete;

    Tfmusic(Tfmusic&& other)
        : m_music(other.m_music), m_name(other.m_name) {}
    Tfmusic& operator=(Tfmusic&& other);

    ~Tfmusic();

    // ============================================================ //
    // Actions
    // ============================================================ //

    /**
     * Call contineously to fill the audio buffer.
     */
    void update() const;

    void play() const;

    void pause() const;

    void resume() const;

    void stop() const;

    /**
     * @param volume Value from 0.0f to 1.0f, will be clamped.
     */
    void set_volume(float volume) const;

    // ============================================================ //
    // Info
    // ============================================================ //

    bool is_playing() const;

    std::string& get_name() { return m_name; }


private:
    Music m_music;
    std::string m_name;
};

}

#endif//__TFMUSIC_HPP__
