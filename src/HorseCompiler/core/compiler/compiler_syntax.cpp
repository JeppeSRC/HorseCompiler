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

#include "compiler.h"
#include <util/util.h>

bool FindTokenCMP(const Token& item, const TokenType& token) {
	return item.type == token;
}

bool FindTypeCMP(const PrimitiveTypeDef& item, const PrimitiveType& type) {
	return item.type == type;
}

void Compiler::SyntaxAnalazys(List<Token>& tokens) {

	for (uint64 i = 0; i < tokens.GetSize(); i++) {
		const Token& t = tokens[i];

		if (t.type == TokenType::Keyword) {
			if (t.keyword == KeywordType::Typedef) {
				i = AnalyzeTypedef(tokens, i);
			}
		}
	}

}

uint64 Compiler::AnalyzeTypedef(List<Token>& tokens, uint64 start) {
	Type* type = nullptr;
	uint64 index = AnalyzeTypeDeclaration(tokens, start + 1, &type);

	if (index++ == ~0) return ~0;

	String name = tokens[index].string;

	Type* tmp = GetType(name);

	if (tmp != nullptr) {
		Compiler::Log(tokens[index], HC_ERROR_SYNTAX_TYPENAME_ALREADY_EXIST, name.str);
	}

	MakeTypeTypeDef(type, name);

	if (tokens[++index].type != TokenType::Semicolon) {
		Compiler::Log(tokens[index], HC_ERROR_SYNTAX_EXPECTED, tokens[index].string.str, ';');
		return ~0;
	}

	return index;
}

uint64 Compiler::AnalyzeTypeDeclaration(List<Token>& tokens, uint64 start, Type** retType) {
	Token signToken;
	uint8 sign = 2;
	uint8 constness = 0;

	Type* tmp = nullptr;
	PrimitiveType type = PrimitiveType::Unknown;

	uint64 i = 0;

	for (i = start; i < tokens.GetSize(); i++) {
		const Token& token = tokens[i];

		if (token.type != TokenType::PrimitiveType) {
			if (token.type == TokenType::Identifier) {
				tmp = GetType(token.string);

				if (tmp == nullptr) {
					break;
				} else if (type != PrimitiveType::Unknown) {
					Compiler::Log(token, HC_ERROR_SYNTAX_TYPE_FOLLOWED_BY_TYPE, GetPrimitiveTypeString(type).str, tmp->name.str);
					return ~0;
				}
			}
		} else {
			switch (token.primitiveType) {
				case PrimitiveType::Const:
					if (constness) {
						Compiler::Log(token, HC_WARN_SYNTAX_SAME_TYPE_QUALIFIER);
					}

					constness = 1;
					break;
				case PrimitiveType::Unsigned:
					if (sign == 0) {
						Compiler::Log(token, HC_WARN_SYNTAX_SAME_TYPE_QUALIFIER);
						return ~0;
					} else if (sign == 1) {
						Compiler::Log(token, HC_ERROR_SYNTAX_SIGNED_UNSIGNED_EXCLUSIVE);
						return ~0;
					}

					sign = 0;
					signToken = token;
					break;
				case PrimitiveType::Signed:
					if (sign == 1) {
						Compiler::Log(token, HC_WARN_SYNTAX_SAME_TYPE_QUALIFIER);
						return ~0;
					} else if (sign == 0) {
						Compiler::Log(token, HC_ERROR_SYNTAX_SIGNED_UNSIGNED_EXCLUSIVE);
						return ~0;
					}

					sign = 1;
					signToken = token;
					break;
				case PrimitiveType::Char:
				case PrimitiveType::Short:
				case PrimitiveType::Int:
				case PrimitiveType::Float:
				case PrimitiveType::Vec2:
				case PrimitiveType::Vec3:
				case PrimitiveType::Vec4:
				case PrimitiveType::Mat4:
					if (type != PrimitiveType::Unknown) {
						Compiler::Log(token, HC_ERROR_SYNTAX_TYPE_FOLLOWED_BY_TYPE, GetPrimitiveTypeString(type).str, GetPrimitiveTypeString(token.primitiveType).str);
						return ~0;
					}

					type = token.primitiveType;
			}
		}

	}

	if (type != PrimitiveType::Unknown) {
		if (type != PrimitiveType::Char && type != PrimitiveType::Short && type != PrimitiveType::Int) {
			if (sign != 2) {
				Compiler::Log(signToken, HC_ERROR_SYNTAX_SIGNED_UNSIGNED_NOT_ALLOWED_ON_TYPE, sign == 0 ? "unsigned" : "signed", GetPrimitiveTypeString(type).str);
				return ~0;
			}
		}

		switch (type) {
			case PrimitiveType::Char:
			case PrimitiveType::Short:
			case PrimitiveType::Int:
			case PrimitiveType::Float:
				*retType = MakeTypeScalar(type, sign, constness);
				break;
			case PrimitiveType::Vec2:
			case PrimitiveType::Vec4:
			case PrimitiveType::Vec3:
				*retType = MakeTypeVec(type, constness);
				break;
			case PrimitiveType::Mat4:
				*retType = MakeTypeMat(type, constness);
		}
	}

	return i-1;
}

