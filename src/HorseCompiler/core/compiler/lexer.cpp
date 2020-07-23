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
	std::vector<int64> indices;
	std::vector<int64> newLines;

	res.tokens.reserve(4096);
	indices.reserve(4096);
	newLines.reserve(4096);

	int64 index = 0;

	for (int64 i = 0; i < syntax.delimiters.length; i++) {
		index = 0;

		while ((index = file.Find(syntax.delimiters[i], index)) != String::npos) {
			indices.push_back(index++);
		}
	}

	index = 0;

	while ((index = file.Find('\n', index)) != String::npos) {
		indices.push_back(index);
		newLines.push_back(index++);
	}

	std::sort(indices.begin(), indices.end());

	int64 currLine = 0;
	int64 lastIndex = 0;

	for (int64 i = 0; i < file.length; i++) {
		char c = file[i];
		for (uint64 j = 0; j < indices.size(); j++) {
			if (i == indices[j]) {
				Token t;
				
				if (lastIndex <= i - 1) {
					t.string = file.SubString(lastIndex, i - 1);
					t.line = currLine + 1;
					t.column = lastIndex - (newLines[currLine] * (currLine > 0)) + 1;

					res.tokens.push_back(t);
				}

				lastIndex = i+1;

				if ((newLines[currLine] - i) == 0) {
					currLine++;
				} else {
					t.column = i - (newLines[currLine] * (currLine > 0)) + 1;
					t.string = c;

					res.tokens.push_back(t);
					break;
				}
			}
		}
		
	}

	return res;
}