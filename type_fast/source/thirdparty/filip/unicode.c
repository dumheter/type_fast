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

#include "unicode.h"

// ========================================================================== //
// Header Includes
// ========================================================================== //

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// ========================================================================== //
// Macro Declarations
// ========================================================================== //

/** First byte mask for 1-byte codepoints **/
#define LN_UTF8_MASK_1BYTE_FIRST 0x7F
/** First byte mask for 2-byte codepoints **/
#define LN_UTF8_MASK_2BYTE_FIRST 0x1F
/** First byte mask for 3-byte codepoints **/
#define LN_UTF8_MASK_3BYTE_FIRST 0x0F
/** First byte mask for 4-byte codepoints **/
#define LN_UTF8_MASK_4BYTE_FIRST 0x07

/** Continuation character mask **/
#define LN_UTF8_CONT_MASK 0x3F

// ========================================================================== //
// Functions
// ========================================================================== //

constexpr int loup_codepoints_size = 3; 
static u32 lowerCodepoints[loup_codepoints_size] = {
  0xE5, 0xE4, 0xF6
};
static u32 upperCodepoints[loup_codepoints_size] = {
  0xC5, 0xC4, 0xD6
};

void lnUTF8ToLower(char8* string, int size)
{
  int pos = 0;
  while (pos < size) {
    u64 codepoint;
    u32 decbytes;
    bool valid = lnUTF8Decode(string, pos, &codepoint, &decbytes);
    assert(valid && "Must be valid");
    assert(decbytes > 1 || (decbytes == 1 && codepoint <= 127) &&
           "codepoint cannot be > 127 if bytes == 1");
    
    if (codepoint <= 127) {
      if (isupper(string[pos])) {
        string[pos] = tolower(string[pos]);
      }
    }
    else {
      int index = 0;
      int is_upper = 0;
      for (; index<loup_codepoints_size; index++) {
        is_upper += codepoint == upperCodepoints[index];
        if (is_upper) break;
      }
      if (is_upper) {
        int encbytes = lnUTF8Encode(string+pos, lowerCodepoints[index]);
        assert(encbytes == decbytes &&
               "decoded bytes is not same length as encoded bytes");
      }
    }

    pos += decbytes;
  }
}

int lnUTF8Seek(const char8* string, int offset)
{
  int pos = offset;
  while (pos --> 0) {
	  unsigned char a = string[pos] & 0b11000000;
	  unsigned char b = 0b10000000;
	  bool c = a != b;
    if (a != b) {
      int wow = 1;
      break;
    }
  }
  return offset - pos;
}

int lnUTF8Encode(char8* out, uint32_t utf)
{
  if (utf <= 0x7F) {
    // Plain ASCII
    out[0] = (char) utf;
    return 1;
  }
  else if (utf <= 0x07FF) {
    // 2-byte unicode
    out[0] = (char) (((utf >> 6) & 0x1F) | 0xC0);
    out[1] = (char) (((utf >> 0) & 0x3F) | 0x80);
    return 2;
  }
  else if (utf <= 0xFFFF) {
    // 3-byte unicode
    out[0] = (char) (((utf >> 12) & 0x0F) | 0xE0);
    out[1] = (char) (((utf >>  6) & 0x3F) | 0x80);
    out[2] = (char) (((utf >>  0) & 0x3F) | 0x80);
    return 3;
  }
  else if (utf <= 0x10FFFF) {
    // 4-byte unicode
    out[0] = (char) (((utf >> 18) & 0x07) | 0xF0);
    out[1] = (char) (((utf >> 12) & 0x3F) | 0x80);
    out[2] = (char) (((utf >>  6) & 0x3F) | 0x80);
    out[3] = (char) (((utf >>  0) & 0x3F) | 0x80);
    return 4;
  }
  else { 
    // error - use replacement character
    out[0] = (char) 0xEF;  
    out[1] = (char) 0xBF;
    out[2] = (char) 0xBD;
    return 0;
  }
}

bool lnUTF8Decode(
	const char8* string, 
	u64 offset, 
	u64* codepoint, 
	u32* numBytes)
{
	// Get the first byte
	const u32 firstByte = (u32)string[offset];

	// Check if no continuation bytes are present (Most common)
	if ((firstByte & 0x80) == 0)
	{
		*codepoint = firstByte;
		*numBytes = 1;
		return true;
	}

	// Check if 1-cont byte
	if ((firstByte & 0xE0) == 0xC0)
	{
		const u32 firstCont = (u32)string[offset + 1] & LN_UTF8_CONT_MASK;
		*codepoint = ((firstByte & LN_UTF8_MASK_2BYTE_FIRST) << 6) | firstCont;
		*numBytes = 2;
		return true;
	}

	// Check if 2-cont bytes
	if ((firstByte & 0xF0) == 0xE0)
	{
		const u32 firstCont = (u32)string[offset + 1] & LN_UTF8_CONT_MASK;
		const u32 secondCont = (u32)string[offset + 2] & LN_UTF8_CONT_MASK;
		*codepoint = ((firstByte & LN_UTF8_MASK_3BYTE_FIRST) << 12) | (firstCont << 6) | secondCont;
		*numBytes = 3;
		return true;
	}

	// Check if 3-cont bytes
	if ((firstByte & 0xF0) == 0xE0)
	{
		const u32 firstCont = (u32)string[offset + 1] & LN_UTF8_CONT_MASK;
		const u32 secondCont = (u32)string[offset + 2] & LN_UTF8_CONT_MASK;
		const u32 thirdCont = (u32)string[offset + 3] & LN_UTF8_CONT_MASK;
		*codepoint = ((firstByte & LN_UTF8_MASK_4BYTE_FIRST) << 18) | (firstCont << 12) | (secondCont << 6) | thirdCont;
		*numBytes = 4;
		return true;
	}

	// Return failure (0 bytes)
	return false;
}

// -------------------------------------------------------------------------- //

u64 lnUTF8StringLength(const char8* string)
{
	// Return 0 if the string is NULL
	if (!string) { return 0; }

	// String bytecount
	const u64 bytecount = strlen(string);

	// Iterate all bytes in the string
	u64 accumulator = 0;
	for (u64 i = 0; i < bytecount; i++)
	{
		const uint8_t byte = (u32)string[i];
		if ((byte & 0xF0) == 0xF0) { i += 3; }
		else if ((byte & 0xE0) == 0xE0) { i += 2; }
		else if ((byte & 0xC0) == 0xC0) { i += 1; }
		accumulator++;
	}

	// Return the length
	return accumulator;
}

// -------------------------------------------------------------------------- //

int64_t lnUTF8IndexOf(const char8* string, u64 codepoint)
{
	// Return -1 if string is NULL
	if (!string) { return -1; }

	// Offset and bytecount
	u64 offset = 0;
	u32 bytecount;

	// Iterate through string
	u64 _codepoint;
	u64 index = 0;
	while (string[offset] != 0)
	{
		// Decode the next byte(s)
		const bool _valid = lnUTF8Decode(string, offset, &_codepoint, &bytecount);
		if (!_valid) { return -1; }

		// Return if codepoint is the one we are looking for
		if (_codepoint == codepoint) { return index; }

		// Increment offset and bytecount
		offset += bytecount;
		index++;
	}

	// Return -1 for failing to find codepoint
	return -1;
}

// -------------------------------------------------------------------------- //

int64_t lnUTF8LastIndexOf(const char8* string, u64 codepoint)
{
	// Return -1 if string is NULL
	if (!string) { return -1; }

	// Offset and bytecount
	u64 offset = 0;
	u32 bytecount;

	// Iterate through string
	u64 _codepoint;
	int64_t foundIndex = -1;
	u64 index = 0;
	while (string[offset] != 0)
	{
		// Decode the next byte(s)
		const bool _valid = lnUTF8Decode(string, offset, &_codepoint, &bytecount);
		if (!_valid) { return foundIndex; }

		// Return if codepoint is the one we are looking for
		if (_codepoint == codepoint) { foundIndex = index; }

		// Increment offset and bytecount
		offset += bytecount;
		index++;
	}

	// Return -1 for failing to find codepoint
	return foundIndex;
}

// -------------------------------------------------------------------------- //

bool lnUTF8StartsWith(const char8* string, u64 codepoint)
{
	// Return false if the string is NULL
	if (!string) { return false; }

	// Decode first codepoint
	u64 _codepoint;
	u32 bytecount;
	const bool valid = lnUTF8Decode(string, 0, &_codepoint, &bytecount);
	if (!valid) { return false; }

	// Return whether the codepoint match
	return (_codepoint == codepoint);
}

// -------------------------------------------------------------------------- //

bool lnUTF8EndsWith(const char8* string, u64 codepoint)
{
	// Offset and bytecount
	u64 offset = 0;
	u32 bytecount;

	// Iterate through string
	u64 _codepoint = 0;
	u64 _index = 0;
	while (string[offset] != 0)
	{
		// Decode the next byte(s)
		const bool _valid = lnUTF8Decode(string, offset, &_codepoint, &bytecount);
		if (!_valid) { return false; }
		offset += bytecount;
		_index++;
	}

	// Return whether codepoint matches
	return (_codepoint == codepoint);
}

// -------------------------------------------------------------------------- //

char8* lnUTF8Substring(const char8* string, u64 from, u64 count)
{
	// Return NULL if string is NULL
	if (!string) { return NULL; }

	// Return empty string if count is zero
	if (count == 0) {
		char* ret = (char*)malloc(1);
		ret[0] = 0;
		return ret; 
	}

	// Offset and bytecount
	u64 startOffset = 0, endOffset = 0;
	u64 offset = 0;
	u32 bytecount;

	// Iterate through string
	u64 _codepoint;
	u64 index = 0;
	while (string[offset] != 0)
	{
		// Decode the next byte(s)
		const bool _valid = lnUTF8Decode(string, offset, &_codepoint, &bytecount);
		if (!_valid) { return NULL; }

		// Setup start offset if we are a specified index
		if (index == from) { startOffset = offset; }
		if (index == from + count) { endOffset = offset; break; }

		// Increment offset and bytecount
		offset += bytecount;
		index++;
	}

	if (string[offset] == 0) { endOffset = offset; }

	// Check that offsets are valid
	if (endOffset == 0) { return NULL; }

	// Take substring with known offsets
	const u64 resultLength = endOffset - startOffset;
	char* buffer = (char*)malloc(sizeof(char) * (resultLength + 1));
	if (!buffer) { return NULL; }
	buffer[resultLength] = 0;
	memcpy(buffer, string + startOffset, resultLength);
	return buffer;
}

// -------------------------------------------------------------------------- //

char8* lnUTF8SubstringFrom(const char8* string, u64 from)
{
	// Return NULL if string is NULL
	if (!string) { return NULL; }

	// Offset and bytecount
	u64 startOffset = 0;
	u64 offset = 0;
	u32 bytecount;

	// Iterate through string
	u64 _codepoint;
	u64 index = 0;
	while (string[offset] != 0)
	{
		// Decode the next byte(s)
		const bool _valid = lnUTF8Decode(string, offset, &_codepoint, &bytecount);
		if (!_valid) { return NULL; }

		// Setup start offset if we are a specified index
		if (index == from) { startOffset = offset; }

		// Increment offset and bytecount
		offset += bytecount;
		index++;
	}

	// Take substring with known offsets
	const u64 resultLength = offset - startOffset;
	char* buffer = (char*)malloc(sizeof(char) * (resultLength + 1));
	if (!buffer) { return NULL; }
	buffer[resultLength] = 0;
	memcpy(buffer, string + startOffset, resultLength);
	return buffer;
}

// -------------------------------------------------------------------------- //

char8* lnUTF8Insert(const char8* string, u64 from, u64 count, const char8* insertion)
{
	assert(false && "NOT_IMPLEMENTED");
	return NULL;
}

// -------------------------------------------------------------------------- //

u64 lnUTF8AtIndex(const char8* string, u64 index)
{
	// Offset and bytecount
	u64 offset = 0;
	u32 bytecount;

	// Iterate through string
	u64 codepoint;
	u64 _index = 0;
	while (string[offset] != 0 && _index < index)
	{
		// Decode the next byte(s)
		const bool _valid = lnUTF8Decode(string, offset, &codepoint, &bytecount);
		if (!_valid) { return 0; }
		offset += bytecount;
		_index++;
	}

	// Decode codepoint
	const bool valid = lnUTF8Decode(string, offset, &codepoint, &bytecount);
	if (!valid) { return 0; }
	return codepoint;
}

// -------------------------------------------------------------------------- //

u64 lnUTF8OffsetOfIndex(const char8* string, u64 index)
{
	// Offset and bytecount
	u64 offset = 0;
	u32 bytecount;

	// Iterate through string
	u64 codepoint;
	u64 _index = 0;
	while (string[offset] != 0 && _index < index)
	{
		// Decode the next byte(s)
		const bool _valid = lnUTF8Decode(string, offset, &codepoint, &bytecount);
		if (!_valid) { return 0; }
		offset += bytecount;
		_index++;
	}

	// Return the offset
	return offset;
}

// -------------------------------------------------------------------------- //

void lnUTF8OffsetOfIndices(const char8* string, u64* indices, u32 indexCount, u32* indicesSet)
{
	// Offset and bytecount
	u64 offset = 0;
	u32 bytecount;

	// Result variables
	*indicesSet = 0;

	// Iterate through string
	u64 codepoint;
	u64 _index = 0;
	while (string[offset] != 0)
	{
		// Decode the next byte(s)
		const bool _valid = lnUTF8Decode(string, offset, &codepoint, &bytecount);
		if (!_valid) { return; }

		// Check if index match
		if (*indicesSet < indexCount && indices[*indicesSet] == _index)
		{
			indices[(*indicesSet)++] = offset;
		}

		// Increment offset and index
		offset += bytecount;
		_index++;
	}
}

// -------------------------------------------------------------------------- //

bool lnUTF8Valid(const char8* string)
{
	// Offset and bytecount
	u64 offset = 0;
	u32 bytecount;

	// Iterate through string
	u64 codepoint;
	u64 index = 0;
	while (string[offset] != 0)
	{
		// Decode the next byte(s)
		const bool validCodepoint = lnUTF8Decode(string, offset, &codepoint, &bytecount);
		if (!validCodepoint) { return false; }
		offset += bytecount;
		index++;
	}
	// Return success
	return true;
}

// ============================================================ //

bool lnUTF8Valid_s(const char8* string, u32 size)
{
	// Offset and bytecount
	u64 offset = 0;
	u32 bytecount;

	// Iterate through string
	u64 codepoint;
	u64 index = 0;
  while (index < size)
  {
    // Decode the next byte(s)
    const bool validCodepoint = lnUTF8Decode(string, offset, &codepoint, &bytecount);
    if (!validCodepoint) { return false; }
    offset += bytecount;
    index++;
  }
  // Return success
  return true;
}

// -------------------------------------------------------------------------- //

bool lnUTF8Iterate(const char8* string, PFN_LnUTF8IterateFunc iterateFunc)
{
	// Offset and bytecount
	u64 offset = 0;
	u32 bytecount;

	// Iterate through string
	u64 codepoint;
	u64 index = 0;
	while (string[offset] != 0)
	{
		// Decode the next byte(s)
		const bool validCodepoint = lnUTF8Decode(string, offset, &codepoint, &bytecount);
		if (!validCodepoint) { return false; }

		// Call the callback
		const bool cont = iterateFunc(codepoint, index, offset);
		if (!cont) { return false; }

		// Increment offset and index
		offset += bytecount;
		index++;
	}

	// Return success
	return true;
}

// ============================================================ //

static inline bool is_valid_letter(unsigned char letter, const DelimSettings* settings)
{
  return (
    (isalpha(letter)) ||
    //(letter > 127) || // covered in ifstatement later
    (settings->allow_numbers && isdigit(letter)) || 
    (settings->allow_punct && ispunct(letter)) ||   
    (settings->allow_hyphen && letter == '-') ||
    (settings->allow_apostrophe && letter == '\'')
    );
}

constexpr u32 allowed_utf8_bigletters_size = 6;
static u32 allowed_utf8_bigletters[allowed_utf8_bigletters_size] = {
  0xE5, 0xE4, 0xF6, 0xC5, 0xC4, 0xD6
};
static bool is_allowed_utf8_bigletter(u32 cp)
{
  for (u32 i=0; i<allowed_utf8_bigletters_size; i++) {
    if (allowed_utf8_bigletters[i] == cp) return true;
  }
  return false;
}

bool lnUTF8FindWord(const char8* string, const DelimSettings* settings, u64* offset, u64* end)
{
  bool valid = true;
  bool found_beginning = false;
  *end = *offset;
  
  while (string[*end] != 0) {
    
    // check for valid utf8 >2 byte chars
    int inc_bytes = 1;
    bool invalid_utf8 = false;
    if ((unsigned char)string[*end] > 127) {
      u64 cp;
      u32 bytes;
      valid = lnUTF8Decode(string, *end, &cp, &bytes);
      if (!valid) { // invalid UTF8 encoding
        *end += 1;
        *offset = *end;
        break;
      }
      invalid_utf8 = !is_allowed_utf8_bigletter((u32)cp);
      inc_bytes = bytes;
    }

    // check for delimit chars
    if (invalid_utf8 || isspace(string[*end]) || 
        !is_valid_letter(string[*end], settings)) {
      
      if (found_beginning) {

        // TODO can it be don without this check?
        if (!lnUTF8Valid_s(&string[*offset], (u32)(*end - *offset))) {
          valid = false;
        }
        
        *end -= 1; // set it to the last byte
        break;
      }
    }
    else {
      found_beginning = true;
    }
      
    *end += inc_bytes;
    if (!found_beginning) *offset = *end; 
  }
  
  return valid;
}

