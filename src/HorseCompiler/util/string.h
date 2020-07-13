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

#pragma once

#include <core/def.h>

class String {
private:
	uint64 length;
	char* str;

public:
	String(const char* const string);
	String(const String& other);
	explicit String(const String* other);
	String(String&& other);
	~String();

	String& operator=(const String& other);
	String& operator=(String&& other);

	bool Equals(const String& other) const;
	bool Equals(const char* const other) const;

	bool StartsWith(const String& other) const;
	bool StartsWith(const char* const other) const;

	bool EndsWith(const String& other) const;
	bool EndsWith(const char* const other) const;

	uint64 Find(const String& other, uint64 offset) const;
	uint64 Find(const char* const other, uint64 offset) const;
	uint64 Find(const char other, uint64 offset) const;
	
	uint64 FindR(const String& other, uint64 offset) const;
	uint64 FindR(const char* const other, uint64 offset) const;
	uint64 FindR(const char other, uint64 offset) const;

	uint64 Count(const String& other) const;
	uint64 Count(const char* const other) const;
	uint64 Count(const char other) const;

	String& Append(const String& other);
	String& Append(const char* const other);

	String& Remove(const String& other);
	String& Remove(const char* const other);
	String& Remove(uint64 start, uint64 end);

	String SubString(uint64 start, uint64 end) const;
	String SubString(const String& start, const String& end) const;
	String SubString(const char* const start, const char* const end) const;

	String& Insert(uint64 start, uint64 end, const String& other);
	String& Insert(uint64 start, uint64 end, const char* const other);
	String& Insert(const String& start, const String& end, const String& other);
	String& Insert(const String& start, const String& end, const char* const other);

	char& operator[](uint64 index);
	char  operator[](uint64 index) const;

	bool operator==(const String& other) const;
	bool operator==(const char* const other) const;

	String operator+(const String& other) const;
	String operator+(const char* const other) const;

	String& operator+=(const String& other);
	String& operator+=(const char* const other);
	
};