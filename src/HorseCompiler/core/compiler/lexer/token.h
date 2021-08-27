/*
MIT License

Copyright (c) 2021 Jesper

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

#include <util/string.h>
#include <core/log/log.h>
#include <core/compiler/sourcelocation.h>
#include <core/compiler/language.h>


struct Token {
	SourceLocation loc;

	String string;

	bool trailingSpace = false; //Set to true if there's a space after this token
	bool isString      = false;

	TokenType     type          = TokenType::Unknown;
	KeywordType   keyword       = KeywordType::Unknown;
	PrimitiveType primitiveType = PrimitiveType::Unknown;
	OperatorType  operatorType  = OperatorType::Unknown;

	// List comp functions
	static bool CharCmp(const Token& item, const char& other) {
		return item.string[0] == other && item.string.length == 1;
	}

	static bool StringCmp(const Token& item, const String& other) {
		return item.string == other;
	}
};

class Tokens : public List<Token> {
private:
public:
	Tokens() : List<Token>() {}
	Tokens(uint64 reserve) : List<Token>(reserve) {}

	Token& operator[](uint64 index) {
		if (index >= GetSize()) {
			Log::Error("unexpected end of file");
			exit(1);
		}

		return items[index];
	}

	const Token& operator[](uint64 index) const {
		if (index >= GetSize()) {
			Log::Error("unexpected end of file");
			exit(1);
		}

		return items[index];
	}
};