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
#include <util/string.h>
#include "lexer.h"

struct Token {
	String string;
	String filename;
	int64 line;
	int64 column;

	bool isString = false;

	bool operator==(const Token& other) const;
};

/*
	Defines a string escape sequence e.g "Escape Sequence \x21". Where \x21 is the escape sequence,
	'x' specifies a byte in hex.

	signature: is in this example the 'x'
	base: is the number system base ie 10 would be regular decimal, 16 hex etc
	value: if base == 0 then the escape sequence has no input and the value in value will
	be placed in the string.
*/

struct EscapeSequence {
	char signature;
	uint8 base = 0;
	uint8 value = 0;
};

class Syntax {
public:
	String delimiters;
	char stringStart;
	char stringEnd;
	char stringEscapeChar;

	uint8 numSequences;
	EscapeSequence* sequence;

public:
	static void Analyze(const Syntax& syntax, Lexer::AnalysisResult& lexerResult);

private:
	static void AnalyzeStrings(const Syntax& syntax, Lexer::AnalysisResult& lexerResult);
	static void AnalyzeEscapeSequence(const Syntax& syntax, Token& token);
};
