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

String::String(char* const str, int64 length) : str(str), length(length) {}

String::String() : length(-1), str(nullptr) { }

String::String(const char* const string) : length(strlen(string)) {
	str = new char[length + 1];
	memcpy(str, string, length+1);
}

String::String(const String& other) : length(other.length) {
	str = new char[length + 1];
	memcpy(str, other.str, length + 1);
}

String::String(const String* other) : length(other->length) {
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
	int64 len = length - other.length;

	if (len < 0) return false;

	return strncmp(str + (length - len), other.str, other.length) == 0;
}

bool String::EndsWith(const char* const other) const {
	return EndsWith(TmpString(other));
}

int64 String::Find(const String& other, int64 offset) const {
	int64 len = length - other.length;

	for (int64 i = offset; i < len; i++) {
		int64 j = 0;
		for (; j < other.length; j++) {
			if (str[i + j] != other.str[j]) break;
		}

		if (j == other.length) return i;
	}

	return npos;
}

int64 String::Find(const char* const other, int64 offset) const {
	return Find(TmpString(other), offset);
}

int64 String::Find(const char other, int64 offset) const {
	for (int64 i = offset; i < length; i++) {
		if (str[i] == other) return i;
	}

	return npos;
}

int64 String::FindR(const String& other, int64 offset) const {
	int64 len = length - other.length;

	for (int64 i = length - offset - 1; i >= offset; i--) {
		int64 j = 0;
		for (; j < other.length; j++) {
			if (str[i + j] != other.str[j]) break;
		}

		if (j == other.length) return i;
	}

	return npos;
}

int64 String::FindR(const char* const other, int64 offset) const {
	return FindR(TmpString(other), offset);
}

int64 String::FindR(const char other, int64 offset) const {
	for (int64 i = length - offset - 1; i >= 0; i--) {
		if (str[i] == other) return i;
	}

	return npos;
}

int64 String::Count(const String& other) const {
	int64 c = 0;

	int64 index = 0;

	while ((index = Find(other, index+1)) != npos) { c++; }

	return c;
}

int64 String::Count(const char* const other) const {
	return Count(TmpString(other));
}

int64 String::Count(const char other) const {
	int64 c = 0;

	int64 index = 0;

	while ((index = Find(other, index + 1)) != npos) { c++; }

	return c;
}

String& String::Append(const String& other) {
	int64 newLen = length + other.length;
	char* tmp = str;

	str = new char[newLen + 1];

	memcpy(str, tmp, length);
	memcpy(str + length, other.str, other.length + 1);

	delete[] tmp;

	length = newLen;

	return *this;
}

String& String::Append(const char* const other) {
	return Append(TmpString(other));
}

String& String::Remove(const String& other) {
	int64 start = Find(other, 0);

	return Remove(start, start+other.length-1);
}

String& String::Remove(const char* const other) {
	return Remove(TmpString(other));
}

String& String::Remove(int64 start, int64 end) {
	char* tmp = str;

	int64 newLen = length - (++end - start);

	str = new char[newLen + 1];

	memcpy(str, tmp, start);
	memcpy(str + start, tmp + end, length - end);

	length = newLen;

	return *this;
}

String String::SubString(int64 start, int64 end) const {
	int64 len = end - start + 1;

	char* tmp = new char[len + 1];

	memcpy(tmp, str + start, len);

	tmp[len] = 0;

	return std::move(String(tmp, len));
}

String String::SubString(const String& start, const String& end) const {
	int64 s = Find(start, 0);
	int64 e = Find(end, s+start.length);

	return std::move(SubString(s, e+end.length));
}

String String::SubString(const char* const start, const char* const end) const {
	return std::move(SubString(TmpString(start), TmpString(end)));
}

String& String::Insert(int64 start, int64 end, const String& other) {
	int64 newLen = length + other.length - (++end - start);

	char* tmp = str;
	str = new char[newLen + 1];

	memcpy(str, tmp, start);
	memcpy(str + start, other.str, other.length);
	memcpy(str + start + other.length, tmp + end, length - end);

	delete[] tmp;

	length = newLen;

	return *this;
}

String& String::Insert(int64 start, int64 end, const char* const other) {
	return Insert(start, end, TmpString(other));
}

String& String::Insert(const String& start, const String& end, const String& other) {
	int64 s = Find(start, 0);
	int64 e = Find(end, s+start.length);

	return Insert(s, e + end.length, other);
}

String& String::Insert(const String& start, const String& end, const char* const other) {
	return Insert(start, end, TmpString(other));
}

void String::ToUpperCase(String& string) {}

void String::ToLowerCase(String& string) {}

char& String::operator[](int64 index) {
	return str[index];
}

char String::operator[](int64 index) const {
	return str[index];
}

bool String::operator==(const String& other) const {
	return Equals(other);
}

bool String::operator==(const char* const other) const {
	return Equals(other);
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