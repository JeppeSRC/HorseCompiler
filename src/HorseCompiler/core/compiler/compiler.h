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

#include "token.h"
#include "language.h"
#include "pattern.h"

class Compiler {
private: 
	String currentDir;
	Language* lang;

public:
	Compiler(const String& currentDir, Language* lang);

	List<Token> LexicalAnalazys(const String& filename);
	void SyntaxAnalazys(List<Token>& lexerResult);

private: // Internal functions
	void AnalyzeStrings(List<Token>& lexerResult);
	void AnalyzeEscapeSequences(Token& token);

	struct PatternResult {
		PatternType type = PatternType::Unknown;
		List<Pattern::PatternItemResult> items;
	};

	void MatchAnyPattern(const Pattern& pattern, List<Token>& tokens, PatternResult* result); // Looks for a pattern
	void MatchPattern(const Pattern::BasePattern* pattern, List<Token>& tokens, PatternResult* result); // Matches a specific pattern
	
public: //static stuff
	static void Log(const Token& item, uint64 code, ...);
};