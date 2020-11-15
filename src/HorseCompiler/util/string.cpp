/*
MIT License

Copyright (c) 2020 Jesper

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE
*/

#include "string.h"
#include <string>
#include <core/error/error.h>

String::String(char* const string, uint64 length) : str(string), length(length) {
	HC_ASSERT(string != nullptr && length > 0);
}

String::String() : length(-1), str(nullptr) { }

String::String(const char* const string) : length(strlen(string)) {
	HC_ASSERT(string != nullptr);
	str = new char[length + 1];
	memcpy(str, string, length+1);
}

String::String(const String& other) : length(other.length) {
	str = new char[length + 1];
	memcpy(str, other.str, length + 1);
}

String::String(const String* other) : length(other->length) {
	HC_ASSERT(other != nullptr);
	str = new char[length + 1];
	memcpy(str, other->str, length + 1);
}

String::String(String&& other) : length(other.length), str(other.str) {
	other.length = -1;
	other.str = 0;
}

String::~String() {
	delete[] str;
}

String& String::operator=(const String& other) {
	delete[] str;
	length = other.length;
	str = new char[length + 1];
	memcpy(str, other.str, length + 1);

	return *this;
}

String& String::operator=(String&& other) {
	delete[] str;
	str = other.str;
	length = other.length;

	other.length = -1;
	other.str = 0;

	return *this;
}

String& String::operator=(const char other) {
	delete[] str;
	str = new char[2];
	str[1] = 0;
	str[0] = other;
	length = 1;

	return *this;
}

bool String::Equals(const String& other) const {
	return Equals(other.str);
}

bool String::Equals(const char* const other) const {
	return strcmp(str, other) == 0;
}

bool String::StartsWith(const String& other) const {
	return strncmp(str, other.str, length) == 0;
}

bool String::StartsWith(const char* const other) const {
	return StartsWith(TmpString(other));
}

bool String::EndsWith(const String& other) const {
	uint64 len = length - other.length;

	if (len < 0) return false;

	return strncmp(str + (length - len), other.str, other.length) == 0;
}

bool String::EndsWith(const char* const other) const {
	return EndsWith(TmpString(other));
}

uint64 String::Find(const String& other, uint64 offset) const {
	HC_ASSERT(offset <= length && offset >= 0);
	uint64 len = length - other.length;

	for (uint64 i = offset; i < len; i++) {
		uint64 j = 0;
		for (; j < other.length; j++) {
			if (str[i + j] != other.str[j]) break;
		}

		if (j == other.length) return i;
	}

	return npos;
}

uint64 String::Find(const char* const other, uint64 offset) const {
	return Find(TmpString(other), offset);
}

uint64 String::Find(const char other, uint64 offset) const {
	HC_ASSERT(offset <= length && offset >= 0);
	for (uint64 i = offset; i < length; i++) {
		if (str[i] == other) return i;
	}

	return npos;
}

uint64 String::FindR(const String& other, uint64 offset, bool offsetFromStart) const {
	HC_ASSERT(offset <= length && offset >= 0);

	uint64 i = offsetFromStart ? length + (offset - length) : length - offset - 1;
	uint64 max = length - other.length;

	if (i > max) i = max;

	for (; i >= offset; i--) {
		uint64 j = 0;
		for (; j < other.length; j++) {
			if (str[i + j] != other.str[j]) break;
		}

		if (j == other.length) return i;
	}

	return npos;
}

uint64 String::FindR(const char* const other, uint64 offset, bool offsetFromStart) const {
	return FindR(TmpString(other), offset);
}

uint64 String::FindR(const char other, uint64 offset, bool offsetFromStart) const {
	HC_ASSERT(offset <= length && offset >= 0);

	uint64 i = offsetFromStart ? length + (offset - length) : length - offset - 1;

	for (; (int64)i >= 0; i--) {
		if (str[i] == other) return i;
	}

	return npos;
}

uint64 String::Count(const String& other, uint64 offset) const {
	uint64 c = 0;

	uint64 index = offset;

	while ((index = Find(other, index+1)) != npos) { c++; }

	return c;
}

uint64 String::Count(const char* const other, uint64 offset) const {
	HC_ASSERT(other != nullptr);
	return Count(TmpString(other), offset);
}

uint64 String::Count(const char other, uint64 offset) const {
	uint64 c = 0;

	uint64 index = 0;

	while ((index = Find(other, index + 1)) != npos) { c++; }

	return c;
}

uint64 String::CountR(const String& other, uint64 offset, bool offsetFromStart) const {
	uint64 c = 0;

	uint64 index = offsetFromStart ? offset + 1 : length - offset;

	while ((index = FindR(other, index - 1, true)) != npos) { c++; }

	return c;
}

uint64 String::CountR(const char* const other, uint64 offset, bool offsetFromStart) const {
	HC_ASSERT(other != nullptr);
	return CountR(TmpString(other), offset, offsetFromStart);
}

uint64 String::CountR(const char other, uint64 offset, bool offsetFromStart) const {
	uint64 c = 0;

	uint64 index = offsetFromStart ? offset + 1 : length - offset;

	while ((index = FindR(other, index - 1, true)) != npos) { c++; }

	return c;
}

String& String::Append(const String& other) {
	uint64 newLen = length + other.length;
	char* tmp = str;

	str = new char[newLen + 1];

	memcpy(str, tmp, length);
	memcpy(str + length, other.str, other.length + 1);

	delete[] tmp;

	length = newLen;

	return *this;
}

String& String::Append(const char* const other) {
	HC_ASSERT(other != nullptr);
	return Append(TmpString(other));
}

String& String::Remove(const String& other) {
	uint64 start = Find(other, 0);

	return Remove(start, start+other.length-1);
}

String& String::Remove(const char* const other) {
	HC_ASSERT(other != nullptr);
	return Remove(TmpString(other));
}

String& String::Remove(uint64 start, uint64 end) {
	HC_ASSERT(start <= end);
	HC_ASSERT(start >= 0 && end < length);
	char* tmp = str;

	uint64 newLen = length - (++end - start);

	str = new char[newLen + 1];

	memcpy(str, tmp, start);
	memcpy(str + start, tmp + end, length - end);

	str[newLen] = 0;

	length = newLen;

	return *this;
}

String& String::RemoveAt(uint64 index) {
	return Remove(index, index);
}

String String::SubString(uint64 start, uint64 end) const {
	HC_ASSERT(start <= end);
	HC_ASSERT(start >= 0 && end < length);
	uint64 len = end - start + 1;

	char* tmp = new char[len + 1];

	memcpy(tmp, str + start, len);

	tmp[len] = 0;

	return std::move(String(tmp, len));
}

String String::SubString(const String& start, const String& end) const {
	uint64 s = Find(start, 0);
	uint64 e = Find(end, s+start.length);

	return std::move(SubString(s, e+end.length));
}

String String::SubString(const char* const start, const char* const end) const {
	HC_ASSERT(start != nullptr && end != nullptr);
	return std::move(SubString(TmpString(start), TmpString(end)));
}

String& String::Replace(uint64 start, uint64 end, const String& other) {
	HC_ASSERT(start <= end);
	HC_ASSERT(start >= 0 && end < length);
	uint64 newLen = length + other.length - (++end - start);

	char* tmp = str;
	str = new char[newLen + 1];

	memcpy(str, tmp, start);
	memcpy(str + start, other.str, other.length);
	memcpy(str + start + other.length, tmp + end, length - end);

	delete[] tmp;

	length = newLen;

	return *this;
}

String& String::Replace(uint64 start, uint64 end, const char* const other) {
	return Replace(start, end, TmpString(other));
}

String& String::Replace(uint64 start, uint64 end, const char other) {
	uint16 tmp = (uint16)other;
	return Replace(start, end, (const char* const)&tmp);
}

String& String::Replace(const String& start, const String& end, const String& other) {
	uint64 s = Find(start, 0);
	uint64 e = Find(end, s+start.length);

	return Replace(s, e + end.length, other);
}

String& String::Replace(const String& start, const String& end, const char* const other) {
	return Replace(start, end, TmpString(other));
}

String& String::Replace(const String& start, const String& end, const char other) {
	uint16 tmp = (uint16)other;
	return Replace(start, end, (const char* const)&tmp);
}

void String::ToUpperCase(String& string) {}

void String::ToLowerCase(String& string) {}

char& String::operator[](uint64 index) {
	return str[index];
}

char String::operator[](uint64 index) const {
	return str[index];
}

bool String::operator==(const String& other) const {
	return Equals(other);
}

bool String::operator==(const char* const other) const {
	return Equals(other);
}

bool String::operator!=(const String& other) const {
	return !Equals(other);
}

bool String::operator!=(const char* const other) const {
	return !Equals(other);
}

String String::operator+(const String& other) const {
	String tmp(str);

	return std::move(tmp.Append(other));
}

String String::operator+(const char* const other) const {
	return std::move(operator+(TmpString(other)));
}

String& String::operator+=(const String& other) {
	return Append(other);
}

String& String::operator+=(const char* const other) {
	return Append(other);
}

TmpString::TmpString(const char* const str) : String(const_cast<char* const>(str), strlen(str)) { }

TmpString::~TmpString() { str = nullptr; }