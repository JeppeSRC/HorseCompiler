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

void ProcessInclude(List<Token>& tokens, uint64 index, const List<String>& includeDir, const Syntax& syntax);

struct PreProcessorData {
	List<std::pair<String, String>> defines;
	List<String> includedFiles;
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

String PreProcessor::Run(Lexer::AnalysisResult& result, List<String>& includeDir, const Syntax& syntax) {
	List<Token>& tokens = result.tokens;

	CorrectIncludeDir(includeDir);
	RemoveComments(tokens);

	for (uint64 i = 0; i < tokens.GetSize(); i++) {
		Token& t = tokens[i];

		if (t.isString) continue;

		char c = t.string[0];

		if (c != '#') continue;

		Token& directive = tokens[i + 1];

		if (directive.string == "include") {
			ProcessInclude(tokens, i-- + 2, includeDir, syntax);
		}

	}



	return std::move(MergeList(tokens, 0, tokens.GetSize() - 1));
}

void ProcessInclude(List<Token>& tokens, uint64 index, const List<String>& includeDir, const Syntax& syntax) {
	Token& t = tokens[index];

	bool local = false;
	char startChar = t.string[0];

	uint64 end = 0;
	uint64 newLine = FindNextNewline(tokens, index);

	if (startChar == '"') {
		local = true;
		end = tokens.Find('"', Token::CharCmp, index + 1);
	} else if (startChar == '<') {
		end = tokens.Find('>', Token::CharCmp, index + 1);
	} else {
		Compiler::Log(t, HC_ERROR_PREPROCESSOR_INCLUDE_UNKNOWN_SYMBOL1, startChar);
	}

	if (newLine == ~0) newLine = tokens.GetSize() - 1;

	if (end == ~0 || end > newLine) {
		Token& n = tokens[newLine];
		Compiler::Log(n, HC_ERROR_PREPROCESSOR_INCLUDE_UNKNOWN_SYMBOL2, n.string[n.string.length - 1], startChar == '<' ? '>' : '"');
	}

	String includeFile = MergeList(tokens, index + 1, end - 1);
	String localPath = StringUtils::GetPathFromFilename(t.filename);
	String finalFile;

	if (local) {
		if (FileUtils::FileExist(localPath + includeFile)) {
			finalFile = localPath + includeFile;
		}
	}

	for (uint64 i = 0; i < includeDir.GetSize(); i++) {
		if (finalFile.length > 0) break;

		const String& dir = includeDir[i];

		if (FileUtils::FileExist(dir + includeFile)) {
			finalFile = dir + includeFile;
		}
	}

	if (finalFile.length == 0) {
		Compiler::Log(t, HC_ERROR_PREPROCESSOR_INCLUDE_FILE_NOT_FOUND, includeFile.str);
	}

	Lexer::AnalysisResult res = Lexer::Analyze(finalFile, syntax);

	index -= 2;

	tokens.Remove(index, newLine);
	tokens.Insert(res.tokens, index);
}