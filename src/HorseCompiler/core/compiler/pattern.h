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
#include <util/list.h>
#include <core/compiler/token.h>

enum class PatternType {
	Unknown,
	Variable,
	Function,

};

class Pattern {
public:
	struct PatternItem {
		PatternItem(TokenType token, const List<PatternItem>& next);
		PatternItem(TokenType token, const PatternItem& next = PatternItem());
		PatternItem(TokenType token, KeywordType keyword, const List<PatternItem>& next);
		PatternItem(TokenType token, KeywordType keyword, const PatternItem& next = PatternItem());
		PatternItem(TokenType token, const List<KeywordType>& keyword, const List<PatternItem>& next);
		PatternItem(TokenType token, const List<KeywordType>& keyword, const PatternItem& next = PatternItem());

		PatternItem(const List<TokenType>& tokens, const List<PatternItem>& next);
		PatternItem(const List<TokenType>& tokens, const PatternItem& next = PatternItem());
		PatternItem(const List<TokenType>& tokens, KeywordType keyword, const List<PatternItem>& next);
		PatternItem(const List<TokenType>& tokens, KeywordType keyword, const PatternItem& next = PatternItem());
		PatternItem(const List<TokenType>& tokens, const List<KeywordType>& keyword, const List<PatternItem>& next);
		PatternItem(const List<TokenType>& tokens, const List<KeywordType>& keyword, const PatternItem& next = PatternItem());

		List<TokenType> tokens; // see below
		List<KeywordType> keywords; // list of possible keywords
		List<PatternItem> next; // is a list of possible items after this one

		/*
		* if tokens has a size of 1 it will either match the token specified unless it's set to one of the following:
		*	Operators: matches only operators
		*	MiscTokens: matches only misc tokens
		*
		* If size is greater than 1 it will match either of the specified tokens
		* */

	private:
		PatternItem() {}
	};


	struct PatternItemResult {
		TokenType token;
		Token data;
	};

	struct BasePattern {
		List<PatternItem> first;
		PatternType type;
	protected:
		BasePattern(PatternType type) : type(type) { }
	};

	struct VariablePattern : public BasePattern {
		VariablePattern();
	} variablePattern;

	struct ParamPattern : public BasePattern {
		ParamPattern();
	};

	struct FunctionDeclarationPattern : public BasePattern {
		ParamPattern param;
		FunctionDeclarationPattern();
	} functionPattern;

};