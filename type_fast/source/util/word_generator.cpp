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

#include "word_generator.hpp"

#include <cassert>

namespace tf
{

Word_generator::Word_generator(const File& file, const DelimSettings& settings)
{
    load(file, settings);
}

void Word_generator::load(const File& file, const DelimSettings& settings)
{
    const char* text = file.get();
    const auto text_size = file.size();
    m_strlist.reserve(text_size/4); // might use extra space, but we can afford it

    u64 end, offset = 0;
    while (offset < text_size) {
        bool valid = lnUTF8FindWord(text, &settings, &offset, &end);
        assert(valid && "failed to parse word");
        std::string str{text+offset, static_cast<size_t>((end+1) - offset)};
        if (settings.only_lowercase) {
            lnUTF8ToLower(&str[0], static_cast<int>(str.size()));
        }
        m_strlist.push_back(std::move(str));

        offset = end + 1;
    }

    std::uniform_int_distribution<long>::param_type param{
        0, static_cast<long>(m_strlist.size() - 1) };
    m_dist.param(param);
}

std::string Word_generator::next()
{
    return m_strlist[m_dist(m_re)];
}

}
