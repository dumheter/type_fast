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
#include <stdio.h>

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
   * Read a file from disk.
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
    
    bool has_error() const { return m_error != FileError::NO_ERROR; }

    const char* get() const { return m_buf; }

    size_t size() const { return m_size; }

    std::string error_to_string() const;

  private:
    FileError m_error = FileError::UNKNOWN_ERROR;
    char* m_buf = nullptr;
    size_t m_size = 0;
  };
}

#endif//__FILE_HPP__
