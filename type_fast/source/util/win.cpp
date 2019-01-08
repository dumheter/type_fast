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

#define MEAN_AND_LEAN
#define NO_MIN_MAX
#include <Windows.h>
#include <Shellapi.h>
#include <cassert>
#include <stdint.h>

namespace tf
{

/**
 * Convert a char string to wchar string.
 * Thx Filip!
 */
static wchar_t* lnUTF8ToUTF16(const char* string)
{
    // if the string is NULL then return
    if (!string) { return NULL; }

    // Get the byte count
    const uint64_t byteCount = strlen(string);
    if (byteCount < 1) { return NULL; }

    // Get the length of the converted string
    const int32_t convertedLength = MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        string,
        (int32_t)byteCount,
        NULL,
        0
        );

    // Assert that the string can be converted
    assert(convertedLength > 0 && "Invalid UTF-8 string");

    // Allocate the utf16 string
    wchar_t* buffer = (wchar_t*)malloc(sizeof(wchar_t) * (convertedLength + 1));
    buffer[convertedLength] = 0;

    // Convert the string
    MultiByteToWideChar(
        CP_UTF8,
        MB_ERR_INVALID_CHARS,
        string,
        (int32_t)byteCount,
        buffer,
        convertedLength
        );

    // Return the utf16 string
    return buffer;
}

void shellexec(const char* cmd)
{
    auto wincmd = lnUTF8ToUTF16(cmd);
    ShellExecuteW(NULL, L"open", wincmd, NULL, NULL, 0);
    free(wincmd);
}

void fix_console()
{
    // Set console encoding
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Enable virtual terminal processing
    const HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(out, &mode);
    SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
}

}
