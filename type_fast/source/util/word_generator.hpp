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

#ifndef __WORD_GENERATOR_HPP__
#define __WORD_GENERATOR_HPP__

// ============================================================ //
// Headers
// ============================================================ //

#include <vector>
#include <random>
#include "file.hpp"
#include "../thirdparty/filip/unicode.h"


// ============================================================ //
// Class
// ============================================================ //

namespace tf
{
  class Word_generator
  {
  public:
    Word_generator(const File& file, const DelimSettings& settings);

    std::string next();

    size_t word_count() const { return m_strlist.size(); }

  private:
    std::vector<std::string> m_strlist;
    std::random_device m_rd{};
    std::default_random_engine m_re;
    std::uniform_int_distribution<long> m_dist;
  };
}

#endif//__WORD_GENERATOR_HPP__
