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

	MiscTokens = 200,

	Comma,
	Semicolon,
	Colon,

	Questionmark,

	ParenthesisOpen,
	ParenthesisClose,
	BracketOpen,
	BracketClose,

	// End of misc tokens
	Others = 300,

	Keyword,
	PrimitiveType,
	Operator,
	Identifier,
	Literal
};

struct TokenTypeDef {
	TokenType type;
	String    def;
};

enum class OperatorType {
	Unknown,

	Dot,
	OpSqBracketOpen,
	OpSqBracketClose,

	OpAssign,
	OpAdd,
	OpSub,
	OpNegate,

	OpMul,
	OpDiv,
	OpCompoundAdd,
	OpCompoundSub,
	OpCompoundMul,
	OpCompoundDiv,

	OpBitNot,
	OpBitAnd,
	OpBitOr,
	OpBitXor,
	OpLeftShift,
	OpRightShift,

	OpNot,
	OpAnd,
	OpOr,
	OpEqual,
	OpNotEqual,

	OpInc,
	OpDec,
	OpPreInc,
	OpPreDec,
	OpPostInc,
	OpPostDec,

	OpLess,
	OpGreater,
	OpLessEq,
	OpGreaterEq,
};

enum class OperatorAssociativity {
	LTR,
	RTL
};

enum class OperandType {
	None,
	//Variable,
	//Value,
	Any
};

struct OperatorTypeDef {
	OperatorType          type = OperatorType::Unknown;
	String                def;
	OperatorAssociativity associativty;
	uint32                precedence;

	OperandType leftOperand  = OperandType::Any;
	OperandType rightOperand = OperandType::Any;
};

enum class KeywordType {
	Unknown,

	Typedef,
	If,
	Else,
	For,
	Return,
	While,
	Switch,
	Struct,
	Extern,
	Layout,
	In,
	Out,
	UniformBuffer,
	Sampler1D,
	Sampler2D,
	Sampler3D
};

struct KeywordDef {
	KeywordType keyword;
	String      def;
};

enum class PrimitiveType {
	Unknown,

	Void,

	Byte,
	Short,
	Int,
	//Long,
	Float,
	//Double,
	Vec2,
	Vec3,
	Vec4,
	Mat4,
	Const,
	Signed,
	Unsigned
};

struct PrimitiveTypeDef {
	PrimitiveType type;
	String        def;
};

class Language {
private:
	bool initialized = false;
public:
	String delimiters;
	char   stringStart;
	char   stringEnd;
	char   stringEscapeChar;
	char   charStart;
	char   charEnd;

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
		char  signature;
		uint8 base = 0;
		uint8 value = 0;
	} *escSequence;

	List<TokenTypeDef> tokenTypes;

	List<KeywordDef>       keywords;
	List<PrimitiveTypeDef> primitiveTypes;
	List<OperatorTypeDef>  operators;

	static Language* Default();
};