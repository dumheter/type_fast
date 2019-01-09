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

#include "tfsound.hpp"

#include <string.h>

namespace tf
{

Tfsound::Tfsound(const char* sound_path)
{
    m_sound = LoadSound(sound_path);

    const long pathlen = static_cast<int>(strlen(sound_path));
    long offset = pathlen;
    while (offset --> 0) {
        if (sound_path[offset] == '/') break;
    }
    m_name = std::string{sound_path+offset+1, static_cast<unsigned long>(pathlen)};
}

Tfsound::~Tfsound()
{
    UnloadSound(m_sound);
}

void Tfsound::play() const
{
    PlaySound(m_sound);
}

void Tfsound::pause() const
{
    PauseSound(m_sound);
}

void Tfsound::resume() const
{
    ResumeSound(m_sound);
}

void Tfsound::stop() const
{
    StopSound(m_sound);
}

bool Tfsound::is_playing() const
{
    return IsSoundPlaying(m_sound);
}

}
