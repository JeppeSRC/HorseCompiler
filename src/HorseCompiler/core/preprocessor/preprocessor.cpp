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

#include "preprocessor.h"

#include <util/file.h>
#include <util/util.h>
#include <core/compiler/compiler.h>

struct PreProcessorData {
	List<std::pair<String, String>> defines;
} data;

void CorrectIncludeDir(List<String>& includeDir) {
	for (String& string : includeDir) {
		StringUtils::ReplaceChar(string, '\\', '/');

		if (!string.EndsWith("/"))
			string.Append("/");
	}
}

uint64 FindNextNewline(const List<Token>& tokens, uint64 index) {
	uint64 line = tokens[index].line;
	const String& file = tokens[index].filename;

	for (uint64 i = index; i < tokens.GetSize(); i++) {
		const Token& t = tokens[i];
		if (t.line != line || t.filename != file) return i-1;
	}

	return ~0;
}

void RemoveComments(List<Token>& tokens) {
	uint64 index = -1;

	while ((index = tokens.Find('/', Token::CharCmp, index+1)) != ~0) {
		char next = tokens[index + 1].string[0];

		if (next == '/') {
			uint64 newLine = FindNextNewline(tokens, index);

			if (newLine == ~0) newLine = tokens.GetSize() - 1;

			tokens.Remove(index, newLine);
		} else if (next == '*') {
			uint64 end = index+2;

			while ((end = tokens.Find('*', Token::CharCmp, end)) != ~0) {
				if (tokens[end += 1].string[0] == '/') break;
			}

			if (end == ~0) end = tokens.GetSize() - 1;

			tokens.Remove(index, end);
		}
	}
}

String MergeList(const List<Token>& tokens, uint64 start, uint64 end) {
	String res(tokens[start].string);

	int64 currentLine = tokens[start].line;

	for (uint64 i =start+1; i <= end; i++) {
		const Token& t = tokens[i];

		if (currentLine < t.line) {
			currentLine = t.line;
			res.Append("\n");
		}

		if (t.isString) {
			res.Append(t.string);
		} else {
			res.Append(" ").Append(t.string);
		}
	}

	return std::move(res);
}

String PreProcessor::Run(Lexer::AnalysisResult& result, List<String>& includeDir) {
	List<Token>& tokens = result.tokens;

	CorrectIncludeDir(includeDir);
	RemoveComments(tokens);

	return std::move(MergeList(tokens));
}
