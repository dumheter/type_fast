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

#include "file.hpp"

namespace tf
{

File::File(const std::string& path)
{
    FILE* file;
    errno_t fopen_s_res = fopen_s(&file, path.c_str(), "rb");
    constexpr errno_t FOPEN_S_SUCCESS = 0;
    if (fopen_s_res == FOPEN_S_SUCCESS) {
        int res = fseek(file, 0, SEEK_END);
        constexpr int SEEK_SUCCESS = 0;
        if (res == SEEK_SUCCESS) {

            m_size = ftell(file);
            constexpr long FTELL_FAIL = -1L;
            if (m_size != FTELL_FAIL) {

                rewind(file);
                m_buf = new char[m_size+1];
                const long long bytes = fread(m_buf, 1, m_size, file);
                if (bytes == m_size) {
                    m_buf[m_size] = 0; // null terminate
                    m_error = FileError::NO_ERROR;
                }
                else {
                    m_error = FileError::FAILED_TO_READ;
                }
            }
            else {
                m_error = FileError::FAILED_TO_GET_POS;
            }
        }
        else {
            m_error = FileError::FAILED_TO_SEEK;
        }
    }
    else {
        m_error = FileError::CANNOT_OPEN_PATH;
    }

    if (file) {
        fclose(file);
    }
}

File::~File()
{
    delete[] m_buf;
}

std::string File::error_to_string() const
{
    switch (m_error)
    {
    case FileError::NO_ERROR: {
        return "NO_ERROR";
    }
    case FileError::CANNOT_OPEN_PATH: {
        return "CANNOT_OPEN_PATH";
    }
    case FileError::FAILED_TO_SEEK: {
        return "FAILED_TO_SEEK";
    }
    case FileError::FAILED_TO_READ: {
        return "FAILED_TO_READ";
    }
    case FileError::FAILED_TO_GET_POS: {
        return "FAILED_TO_GET_POS";
    }
    case FileError::UNKNOWN_ERROR: {
        return "UNKNOWN_ERROR";
    }
    }

    return "UNKNOWN_ERROR"; // have to repeat myself
}

}
