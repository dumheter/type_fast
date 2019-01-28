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

// allow us to use fopen
#pragma warning(disable : 4996)

namespace tf
{

File::File(const std::string& path)
{
    FILE* file = fopen(path.c_str(), "rb");
    if (file) {
        int res = fseek(file, 0, SEEK_END);
        constexpr int SEEK_SUCCESS = 0;
        if (res == SEEK_SUCCESS) {

            this->size = ftell(file);
            constexpr size_t FTELL_FAIL = -1L;
            if (this->size != FTELL_FAIL) {

                rewind(file);
                this->buf = new char[this->size+1];
                const size_t bytes = fread(this->buf, 1, this->size, file);
                if (bytes == this->size) {
                    this->buf[this->size] = 0; // null terminate
                    this->error = FileError::NO_ERROR;
                }
                else {
                    this->error = FileError::FAILED_TO_READ;
                }
            }
            else {
                this->error = FileError::FAILED_TO_GET_POS;
            }
        }
        else {
            this->error = FileError::FAILED_TO_SEEK;
        }
    }
    else {
        this->error = FileError::CANNOT_OPEN_PATH;
    }

    if (file) {
        fclose(file);
    }
}

File::~File()
{
    delete[] this->buf;
}

std::string File::error_to_string() const
{
    switch (this->error)
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

bool File::file_exists(const std::string& path)
{
    FILE* file = fopen(path.c_str(), "rb");
    bool exists = file;
    if (file) { fclose(file); }
    return exists;
}

}
