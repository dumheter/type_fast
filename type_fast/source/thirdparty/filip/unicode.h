// MIT License
//
// Copyright (c) 2018-2019 Filip Bj�rklund
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef LN_UNICODE_H
#define LN_UNICODE_H

#include <cstdint>

using u64 = uint64_t;
using u32 = uint32_t;
using s64 = int64_t;
using s32 = int32_t;
using char8 = char;

// ========================================================================== //
// Functions
// ========================================================================== //

/** Function that acts as a callback for the UTF-8 iteration function. This
 * function is called for each codepoint in the string.
 * \brief UTF-8 codepoint iterate function.
 * \param codepoint Codepoint.
 * \param byteOffset Byte offset of the codepoint in the string.
 * \param index Index of codepoint in string.
 * \return Return true to continue iteration and return false to stop.
 */
typedef bool(*PFN_LnUTF8IterateFunc)(
	u64 codepoint,
	u64 byteOffset,
	u64 index
	);

/**
 * Structure used to define what is considered a delimit character.
 */
typedef struct
{
  bool only_lowercase;       // convert upper case to lower
  bool allow_apostrophe;     // it's
  bool allow_hyphen;         // one-word
  bool allow_punct;          // !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
  bool allow_numbers;        // 1234567890
} DelimSettings;

// ========================================================================== //
// UTF-8 Functions
// ========================================================================== //

void lnUTF8ToLower(char8* string, int size);

/**
 * Seek backwards one code point.
 *
 * @param string
 * @param offset
 * @return 
 */
int lnUTF8Seek(const char8* string, int offset);

/**
 * Encode a code point using UTF-8
 * 
 * @param out - output buffer (min 4 characters)
 * @param utf - code point 0-0x10FFFF
 * @return number of bytes on success, 0 on failure (also produces U+FFFD, which uses 3 bytes)
 */
int lnUTF8Encode(char *out, uint32_t utf);

/** Decode a codepoint a UTF-8 string at the specified offset. The codepoint and
 * number of bytes that it's encoded in is returned in parameter 3 and 4.
 * \note If the function fails to decode a codepoint then the codepoint
 * parameter is not overwritten.
 * \brief Decode UTF-8 codepoint
 * \param string String to decode codepoint in.
 * \param offset Offset to decode at in string.
 * \param codepoint Codepoint.
 * \param numBytes Number of bytes that codepoint is encoded in.
 * \return True if a valid UTF-8 codepoint could be decoded else false.
 */
bool lnUTF8Decode(
  const char8* string, 
	u64 offset, 
	u64* codepoint, 
	u32* numBytes);

// -------------------------------------------------------------------------- //

/** Returns the length of an UTF-8 encoded string in number of codepoints.
 * \brief Returns UTF-8 string length.
 * \param string String to get length of.
 * \return Length of string.
 */
u64 lnUTF8StringLength(const char8* string);

// -------------------------------------------------------------------------- //

/** Returns the index of the first occurance of a codepoint in a UTF-8 encoded
* string. If the codepoint is not in the string then -1 is returned instead.
* \brief Returns first index of codepoint.
* \param string String.
* \param codepoint Codepoint to get index of.
* \return Index of codepoint or -1 if codepoint was not found in string.
*/
s64 lnUTF8IndexOf(const char8* string, u64 codepoint);

// -------------------------------------------------------------------------- //

/** Returns the index of the last occurance of a codepoint in a UTF-8 encoded
 * string. If the codepoint is not in the string then -1 is returned instead.
 * \note -1 is also returned if string is NULL.
 * \brief Returns last index of codepoint.
 * \param string String.
 * \param codepoint Codepoint to get index of.
 * \return Index of codepoint of -1 if codepoint was not found in string.
 */
s64 lnUTF8LastIndexOf(const char8* string, u64 codepoint);

// -------------------------------------------------------------------------- //

/** Returns whether or not a UTF-8 encoded string starts with the specified
 * codepoint.
 * \note False is returned if string is NULL.
 * \brief Returns whether UTF-8 string starts with codepoint.
 * \param string String to check if starts with codepoint.
 * \param codepoint Codepoint to check if string starts with.
 * \return True if the string starts with the codepoint else false.
 */
bool lnUTF8StartsWith(const char8* string, u64 codepoint);

// -------------------------------------------------------------------------- //

/** Returns whether or not a UTF-8 encoded string ends with the specified
 * codepoint.
 * \note False is returned if string is NULL.
 * \brief Returns whether UTF-8 string ends with codepoint.
 * \param string String to check if ends with codepoint.
 * \param codepoint Codepoint to check if string ends with.
 * \return True if the string ends with the codepoint else false.
 */
bool lnUTF8EndsWith(const char8* string, u64 codepoint);

// -------------------------------------------------------------------------- //

/** Returns substring of a UTF-8 encoded string from the specified 'from' index
 * and 'count' number of indices forward.
 * \note Returns NULL if string is NULL.
 * \note A sum of 'from' and 'count' greater than the lenght of the string will
 * result in undefined behaviour.
 * \brief Returns UTF-8 substring.
 * \param string String to get substring of.
 * \param from Index to get substring from.
 * \param count Number of codepoints in substring.
 * \return Substring or NULL on failure.
 */
char8* lnUTF8Substring(const char8* string, u64 from, u64 count);

// -------------------------------------------------------------------------- //

/** Returns substring of a UTF-8 encoded string from the specified 'from' index
 * to the end of the string.
 * \note Returns NULL if string is NULL.
 * \brief Returns UTF-8 substring.
 * \param string String to get substring of.
 * \param from Index to get substring from.
 * \return Substring or NULL on failure.
 */
char8* lnUTF8SubstringFrom(const char8* string, u64 from);

// -------------------------------------------------------------------------- //

char8* lnUTF8Insert(const char8* string, u64 from, u64 count, const char8* insertion);

// -------------------------------------------------------------------------- //

/** Returns the codepoint at a specified index in a UTF-8 encoded string.
 * \brief Returns codepoint at index in UTF-8 string.
 * \param string String to get codepoint at index from.
 * \param index Index to get codepoint at.
 * \return Codepoint at index.
 */
u64 lnUTF8AtIndex(const char8* string, u64 index);

// -------------------------------------------------------------------------- //

/** Returns the byte offset for a specified codepoint index in a UTF-8 encoded
 * string.
 * \note Returns 0 if the string is NULL.
 * \brief Returns byte offset of codepoint index.
 * \param string String to get byte offset of index in.
 * \param index Index to get byte offset of.
 * \return Byte offset of index.
 */
u64 lnUTF8OffsetOfIndex(const char8* string, u64 index);

// -------------------------------------------------------------------------- //

/** Maps each of the indices in the 'indices' parameter to their respective byte
 * offset in the string. The indices must be strictly increasing. The number of
 * indices that was written will be returned in the 'indicesSet' parameter.
 * \brief Maps indices to offsets in UTF-8 string.
 * \param[in] string String to map indices in.
 * \param[in,out] indices Indices to map to offsets. Offsets are written over
 * the indices of the input
 * \param[in] indexCount Number of indices in list.
 * \param[out] indicesSet Returns the number of indices set.
 */
void lnUTF8OffsetOfIndices(const char8* string, u64* indices, u32 indexCount, uint32_t* indicesSet);

// -------------------------------------------------------------------------- //

/** Returns whether or not a string is a valid UTF-8 encoded string.
 * \brief Returns whether string are valid UTF-8.
 * \param string String to check if valid UTF-8.
 * \return True if the string is valid UTF-8 else false.
 */
bool lnUTF8Valid(const char8* string);

// -------------------------------------------------------------------------- //

/** Returns whether or not a string is a valid UTF-8 encoded string.
 * \brief Returns whether string are valid UTF-8.
 * \param string String to check if valid UTF-8.
 * \param size Size of string in bytes.
 * \return True if the string is valid UTF-8 else false.
 */
bool lnUTF8Valid_s(const char8* string, u32 size);

// -------------------------------------------------------------------------- //

/** Iterates through all the codepoints in a UTF-8 encoded string and calls the
 * specified callback function with the codepoint, byte offset and index.
 * \brief Iterate through UTF-8 string.
 * \param string String to iterate through.
 * \param iterateFunc Iterator function.
 * \return True if the complete string was iterated through. False if the
 * iterateFunc callback returned false or if an error occured such as the string
 * is not valid UTF-8.
 */
bool lnUTF8Iterate(const char8* string, PFN_LnUTF8IterateFunc iterateFunc);

/**
 * Will look at the string, starting at the offset, for a word. A word is a set
 * of characters that does not include delimiter characters. What a delimiter
 * character is, is defined in @settings. For example a space, ' ', is a delimiter.
 *
 * @param string String to iterate through.
 * @param settings Settings that will decide what is considered a delimiter.
 * @param offset Where to begin in the string.
 * @param end Where the last byte of the word is.
 * @return If a valid UTF8 word could be formed from the input.
 */
bool lnUTF8FindWord(const char8* string, const DelimSettings* settings, u64* offset, u64* end);


// ========================================================================== //
// End of Header
// ========================================================================== //

#endif // LN_UNICODE_H
