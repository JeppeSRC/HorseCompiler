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

#include "language.h"

Language* Language::Default() {
    static Language lang;

	if (lang.initialized) return &lang;

	lang.delimiters = " #=+-*/<>.,^&|(){}[]%\"'!?:;";
	lang.stringStart = '"';
	lang.stringEnd = '"';
	lang.charStart = '\'';
	lang.charEnd = '\'';
	lang.numSequences = 2;
	lang.escSequence = new EscapeSequence[3];
	lang.escSequence[0].signature = 'n';
	lang.escSequence[0].value = '\n';
	lang.escSequence[1].signature = 'x';
	lang.escSequence[1].base = 16;

	auto& operators = lang.operators;

	operators.PushBack({ OperatorType::Dot, ".", OperatorAssociativity::LTR, 1 });
	operators.PushBack({ OperatorType::OpInc, "++", OperatorAssociativity::LTR, 1, OperandType::Any, OperandType::None });
	operators.PushBack({ OperatorType::OpDec, "--", OperatorAssociativity::LTR, 1, OperandType::Any, OperandType::None });
	operators.PushBack({ OperatorType::OpSqBracketOpen, "[", OperatorAssociativity::LTR, 1 });
	operators.PushBack({ OperatorType::OpSqBracketClose, "]", OperatorAssociativity::LTR, 1 });

	operators.PushBack({ OperatorType::OpInc, "++", OperatorAssociativity::RTL, 2, OperandType::None, OperandType::Any });
	operators.PushBack({ OperatorType::OpDec, "--", OperatorAssociativity::RTL, 2, OperandType::None, OperandType::Any });
	operators.PushBack({ OperatorType::OpNegate, "-", OperatorAssociativity::RTL, 2, OperandType::None, OperandType::Any });
	operators.PushBack({ OperatorType::OpBitNot, "~", OperatorAssociativity::RTL, 2, OperandType::None, OperandType::Any });
	operators.PushBack({ OperatorType::OpNot, "!", OperatorAssociativity::RTL, 2, OperandType::None, OperandType::Any });

	operators.PushBack({ OperatorType::OpMul, "*", OperatorAssociativity::LTR, 3 });
	operators.PushBack({ OperatorType::OpDiv, "/", OperatorAssociativity::LTR, 3 });

	operators.PushBack({ OperatorType::OpAdd, "+", OperatorAssociativity::LTR, 4 });
	operators.PushBack({ OperatorType::OpSub, "-", OperatorAssociativity::LTR, 4 });

	operators.PushBack({ OperatorType::OpLeftShift, "<<", OperatorAssociativity::LTR, 5 });
	operators.PushBack({ OperatorType::OpRightShift, ">>", OperatorAssociativity::LTR, 5 });

	operators.PushBack({ OperatorType::OpLess, "<", OperatorAssociativity::LTR, 6 });
	operators.PushBack({ OperatorType::OpGreater, ">", OperatorAssociativity::LTR, 6 });
	operators.PushBack({ OperatorType::OpLessEq, "<=", OperatorAssociativity::LTR, 6 });
	operators.PushBack({ OperatorType::OpGreaterEq, ">=", OperatorAssociativity::LTR, 6 });

	operators.PushBack({ OperatorType::OpEqual, "==", OperatorAssociativity::LTR, 7 });
	operators.PushBack({ OperatorType::OpNotEqual, "!=", OperatorAssociativity::LTR, 7 });

	operators.PushBack({ OperatorType::OpBitAnd, "&", OperatorAssociativity::LTR, 8 });

	operators.PushBack({ OperatorType::OpBitXor, "^", OperatorAssociativity::LTR, 9 });

	operators.PushBack({ OperatorType::OpBitOr, "|", OperatorAssociativity::LTR, 10 });

	operators.PushBack({ OperatorType::OpAnd, "&&", OperatorAssociativity::LTR, 11 });

	operators.PushBack({ OperatorType::OpOr, "||", OperatorAssociativity::LTR, 12 });

	operators.PushBack({ OperatorType::OpAssign, "=", OperatorAssociativity::RTL, 14, OperandType::Any });
	operators.PushBack({ OperatorType::OpCompoundAdd, "+=", OperatorAssociativity::RTL, 14, OperandType::Any });
	operators.PushBack({ OperatorType::OpCompoundSub, "-=", OperatorAssociativity::RTL, 14, OperandType::Any });
	operators.PushBack({ OperatorType::OpCompoundMul, "*=", OperatorAssociativity::RTL, 14, OperandType::Any });
	operators.PushBack({ OperatorType::OpCompoundDiv, "/=", OperatorAssociativity::RTL, 14, OperandType::Any });

	List< TokenTypeDef >& type = lang.tokenTypes;

	type.PushBack({ TokenType::Comma, "," });
	type.PushBack({ TokenType::Semicolon, ";" });
	type.PushBack({ TokenType::Colon, ":" });

	type.PushBack({ TokenType::Questionmark, "?" });

	type.PushBack({ TokenType::ParenthesisOpen, "(" });
	type.PushBack({ TokenType::ParenthesisClose, ")" });
	type.PushBack({ TokenType::BracketOpen, "{" });
	type.PushBack({ TokenType::BracketClose, "}" });

	type.PushBack({ TokenType::Semicolon, ";" });

	auto& primitiveTypes = lang.primitiveTypes;

	primitiveTypes.PushBack({ PrimitiveType::Void, "void" });
	primitiveTypes.PushBack({ PrimitiveType::Byte, "byte" });
	primitiveTypes.PushBack({ PrimitiveType::Short, "short" });
	primitiveTypes.PushBack({ PrimitiveType::Int, "int" });
	//	primitiveTypes.PushBack({ PrimitiveType::Long, "long" });
	primitiveTypes.PushBack({ PrimitiveType::Float, "float" });
	//	primitiveTypes.PushBack({ PrimitiveType::Double, "double" });
	primitiveTypes.PushBack({ PrimitiveType::Vec2, "vec2" });
	primitiveTypes.PushBack({ PrimitiveType::Vec3, "vec3" });
	primitiveTypes.PushBack({ PrimitiveType::Vec4, "vec4" });
	primitiveTypes.PushBack({ PrimitiveType::Mat4, "mat4" });
	primitiveTypes.PushBack({ PrimitiveType::Const, "const" });
	primitiveTypes.PushBack({ PrimitiveType::Signed, "signed" });
	primitiveTypes.PushBack({ PrimitiveType::Unsigned, "unsigned" });

	auto& keywords = lang.keywords;

	keywords.PushBack({ KeywordType::Typedef, "typedef" });

	keywords.PushBack({ KeywordType::If, "if" });
	keywords.PushBack({ KeywordType::Else, "else" });
	keywords.PushBack({ KeywordType::For, "for" });
	keywords.PushBack({ KeywordType::While, "while" });
	keywords.PushBack({ KeywordType::Switch, "switch" });
	keywords.PushBack({ KeywordType::Return, "return" });

	keywords.PushBack({ KeywordType::Struct, "struct" });

	keywords.PushBack({ KeywordType::Extern, "extern" });

	keywords.PushBack({ KeywordType::Layout, "layout" });
	keywords.PushBack({ KeywordType::In, "in" });
	keywords.PushBack({ KeywordType::Out, "out" });
	keywords.PushBack({ KeywordType::UniformBuffer, "UniformBuffer" });
	keywords.PushBack({ KeywordType::Sampler1D, "Sampler1D" });
	keywords.PushBack({ KeywordType::Sampler2D, "Sampler2D" });
	keywords.PushBack({ KeywordType::Sampler3D, "Sampler3D" });


	lang.initialized = true;

	return &lang;
}