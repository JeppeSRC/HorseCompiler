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


enum class TokenType {
	/*
	* Each group is allocated 100 numbers:
	*	Operators 100-199
	*	MiscTokens 200-299
	*   Others 300+
	* */

	Unknown,

	Operators = 100,
	OpAssign,

	Plus,
	Minus,
	Asterix,
	Slash,
	OpCompoundAdd,
	OpCompoundSub,
	OpCompoundMul,
	OpCompoundDiv,

	OpBitNot,
	Ampersand,
	OpBitOr,
	OpBitXOr,
	OpLeftShift,
	OpRightShift,

	OpNot,
	OpAnd,
	OpOr,
	OpEqual,
	OpNotEqual,

	OpInc,
	OpDec,

	OpLess,
	OpGreater,
	OpLessEq,
	OpGreaterEq,

	MiscTokens = 200,

	Dot,
	Comma,
	Semicolon,
	Colon,

	Questionmark,

	ParenthesisOpen,
	ParenthesisClose,
	BracketOpen,
	BracketClose,
	SqBracketOpen,
	SqBracketClose,

	// End of misc tokens
	Others = 300,

	Keyword,
	Identifier,
	Literal
};


struct TokenTypeDef {
	TokenType type;
	String def;
};

enum class KeywordType {
	Unknown,

	Char,
	Short,
	Int,
	Long,
	Float,
	Double,
	Vec2,
	Vec3,
	Vec4,
	Mat4,
	Const,
	Signed,
	Unsigned,
	Typedef,
	If,
	For,
	While,
	Switch,
	Struct,
	Extern,
	Layout,
	In,
	Out
};

struct KeywordDef {
	KeywordType keyword;
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
};

class Language {
public:
	Syntax syntax;
	List<KeywordDef> keywords;
};