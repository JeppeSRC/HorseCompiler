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

#include <util/string.h>
#include <util/list.h>

/*
struct Keyword {
	enum Type {
		Unknown,

		//Control Flow
		If,
		Else,
		For,
		Do,
		While,
		Switch,
		Case,
		Continue,
		Break,
		Default,
		Return,
		Goto,

		//Primitive Types
		Void,
		Char,
		Short,
		Int,
		Long,
		Float,
		Double,

		//Other Type Stuff
		Unsigned,
		Signed,
		Auto,
		Typedef,
		Sizeof,
		Const,
		Struct,

	} type;

	String string;
	
};

enum class TokenType;
struct Operator { 
	TokenType type;
	String string;
};
*/

enum class TokenType;
struct TokenTypeDef {
	TokenType type;
	String def;
};

struct Syntax {
	String delimiters;
	char stringStart;
	char stringEnd;
	char stringEscapeChar;
	char charStart;
	char charEnd;


	uint8 numSequences;

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
	} *escSequence;

	List<TokenTypeDef> tokenTypes;
	//List<Keyword> keywords;
	//List<Operator> operators; // Defines operators and punctuation characters
};

class Language {
public:
	Syntax syntax;
};