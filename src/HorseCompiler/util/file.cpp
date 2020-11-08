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

#include "file.h"
#include <stdio.h>
#include <string>

String FileUtils::LoadTextFile(const String& filename) {
	uint64 size = 0;
	byte* tmp = LoadFile(filename, &size);

	return std::move(String((char* const)tmp, size));
}

byte* FileUtils::LoadFile(const String& filename, uint64* size) {
	FILE* file = fopen(filename.str, "rb");

	if (!file) return nullptr;

	fseek(file, 0, SEEK_END);
	*size = (uint64)ftell(file);
	fseek(file, 0, SEEK_SET);

	byte* data = new byte[*size];

	fread(data, *size, 1, file);
	fclose(file);

	return data;
}

bool FileUtils::FileExist(const String& filename) {
	FILE* file = fopen(filename.str, "rb");

	bool res = file != nullptr;

	fclose(file);

	return res;
}