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
#include <algorithm>


Lexer::AnalysisResult Lexer::Analyze(String file, const Syntax& syntax) {
	Lexer::AnalysisResult res;
	List<uint64> indices;
	List<uint64> newLines;

	res.tokens.Reserve(4096);
	indices.Reserve(4096);
	newLines.Reserve(4096);

	uint64 index = 0;

	for (uint64 i = 0; i < syntax.delimiters.length; i++) {
		index = 0;

		while ((index = file.Find(syntax.delimiters[i], index)) != String::npos) {
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

	for (uint64 i = 0; i < file.length; i++) {
		char c = file[i];
		for (uint64 j = 0; j < indices.GetSize(); j++) {
			if (i == indices[j]) {
				Token t;
				
				if (lastIndex <= i - 1) {
					t.string = file.SubString(lastIndex, i - 1);
					t.line = currLine + 1;
					t.column = lastIndex - ((newLines[currLine - 1 * (currLine > 0)] + 1) * (currLine > 0)) + 1;

					res.tokens.PushBack(t);
				}

				lastIndex = i+1;

				if ((newLines[currLine] - i) == 0) {
					currLine++;
					break;
				} else {
					t.column = i - ((newLines[currLine - 1 * (currLine > 0)] + 1) * (currLine > 0)) + 1;
					t.string = c;

					res.tokens.PushBack(t);
					break;
				}
			}
		}
		
	}

	return res;
}