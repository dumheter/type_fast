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

#include "tfmusic.hpp"

#include "../util/util.hpp"

namespace tf
{

Tfmusic::Tfmusic(const char* music_path)
{
    m_music = LoadMusicStream(music_path);

    const long pathlen = static_cast<int>(strlen(music_path));
    long offset = pathlen;
    while (offset --> 0) {
        if (music_path[offset] == '/') break;
    }
    m_name = std::string{music_path+offset+1, static_cast<unsigned long>(pathlen)};
}

Tfmusic& Tfmusic::operator=(Tfmusic&& other)
{
    if (this != &other) {
        m_music = other.m_music;
        other.m_music = nullptr;
        m_name = other.m_name;
    }
    return *this;
}

Tfmusic::~Tfmusic()
{
    if (m_music) UnloadMusicStream(m_music);
}

void Tfmusic::update() const
{
    UpdateMusicStream(m_music);
}

void Tfmusic::play() const
{
    PlayMusicStream(m_music);
}

void Tfmusic::pause() const
{
    PauseMusicStream(m_music);
}

void Tfmusic::resume() const
{
    ResumeMusicStream(m_music);
}

void Tfmusic::stop() const
{
    StopMusicStream(m_music);
}

bool Tfmusic::is_playing() const
{
    return IsMusicPlaying(m_music);
}

void Tfmusic::set_volume(float volume) const
{
    SetMusicVolume(m_music, clamp(volume, 0.0f, 1.0f));
}

}
