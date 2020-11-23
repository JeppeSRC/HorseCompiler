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


#include "lexer.h"
#include "syntax.h"
#include "compiler.h"

#include <util/file.h>
#include <util/util.h>

#include <algorithm>

#define IN_STRING 0x01
#define IN_INCLUDE 0x02

Lexer::AnalysisResult Lexer::Analyze(const String& filename) {
	Lexer::AnalysisResult res;
	List<uint64> indices;
	List<uint64> newLines;

	String file = FileUtils::LoadTextFile(filename);

	res.tokens.Reserve(4096);
	indices.Reserve(4096);
	newLines.Reserve(4096);

	res.tokens.PushBack(Token());

	Syntax* syntax = Compiler::GetSyntax();

	uint64 index = 0;

	for (uint64 i = 0; i < syntax->delimiters.length; i++) {
		index = 0;

		while ((index = file.Find(syntax->delimiters[i], index)) != String::npos) {
			indices.PushBack(index++);
		}
	}

	index = 0;

	while ((index = file.Find('\n', index)) != String::npos) {
		indices.PushBack(index);
		newLines.PushBack(index++);
	}

	std::sort(indices.begin(), indices.end());

	uint64 currLine = 0;
	uint64 lastIndex = 0;

	uint8 includeSpaces = false;
	bool setNextSpace = true;

	for (uint64 i = 0; i < file.length; i++) {
		char c = file[i];
		for (uint64 j = 0; j < indices.GetSize(); j++) {
			if (i == indices[j]) {
				Token t;
				
				if ((int64)lastIndex <= (int64)i - 1) {
					t.filename = filename;
					t.string = file.SubString(lastIndex, i - 1);
					t.line = currLine + 1;
					t.column = lastIndex - ((newLines[currLine - 1 * (currLine > 0)] + 1) * (currLine > 0)) + 1;
					t.isString = includeSpaces;

					uint64 tmp = 0;

					while ((tmp = t.string.Find('\t', tmp)) != ~0) {
						t.string.RemoveAt(tmp);
					}

					if (t.string.length > 0) {
						res.tokens.PushBack(t);
					}

				}

				lastIndex = i+1;

				if ((newLines[currLine] - i) == 0) {
					currLine++;
					break;
				} else {

					t.filename = filename;
					t.column = i - ((newLines[currLine - 1 * (currLine > 0)] + 1) * (currLine > 0)) + 1;
					t.line = currLine + 1;
					t.string = c;
					t.isString = (bool)includeSpaces;

					if (c == '"' && file[i - 1] != '\\') {
						if (includeSpaces == IN_STRING) {
							t.isString = true;
							includeSpaces = 0;
						} else {
							includeSpaces = IN_STRING;
						}
					} else if (includeSpaces == 0 && c == '<') {
						includeSpaces = IN_INCLUDE;
					} else if (includeSpaces == IN_INCLUDE && c == '>') {
						t.isString = false;
						includeSpaces = 0;
					}

					if (includeSpaces) {
						res.tokens.PushBack(t);
					} else if (t.string == " ") {
						if (!setNextSpace) continue;
						res.tokens[res.tokens.GetSize() - 1].trailingSpace = true;
						setNextSpace = false;
					} else {
						res.tokens.PushBack(t);
						setNextSpace = true;
					}

					break;
				}
			}
		}
		
	}

	res.tokens.Remove(0, 0);

	return res;
}
