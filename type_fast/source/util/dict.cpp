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

#include "dict.hpp"

#include <cctype>
#include <cassert>

namespace tf
{
Dict::Dict(const File& file, const DelimSettings& settings)
{
    const char* text = file.get();
    const auto text_size = file.size();

    u64 end, offset = 0;
    while (offset < text_size) {
        bool valid = lnUTF8FindWord(text, &settings, &offset, &end);
        assert(valid && "failed to parse word");
        std::string str{text+offset, static_cast<size_t>((end+1) - offset)};
        if (settings.only_lowercase) {
            lnUTF8ToLower(&str[0], static_cast<int>(str.size()));
        }
        m_set.insert(std::move(str));

        offset = end + 1;
    }
}
}
