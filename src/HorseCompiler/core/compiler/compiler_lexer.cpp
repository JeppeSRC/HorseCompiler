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

#include <util/file.h>
#include <util/util.h>

#include <algorithm>

#define IN_STRING 0x01
#define IN_INCLUDE 0x02

List<Token> Compiler::LexicalAnalazys(const String& filename) {
	List<Token> result;
	List<uint64> indices;
	List<uint64> newLines;

	String file = FileUtils::LoadTextFile(filename);

	result.Reserve(4096);
	indices.Reserve(4096);
	newLines.Reserve(4096);

	result.PushBack(Token());

	uint64 index = 0;

	for (uint64 i = 0; i < lang->syntax.delimiters.length; i++) {
		index = 0;

		while ((index = file.Find(lang->syntax.delimiters[i], index)) != String::npos) {
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
						t.trailingSpace = file[i] == ' ';
						result.PushBack(t);
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
						result.PushBack(t);
					} else if (t.string == " ") {
						if (!setNextSpace) continue;
						result[result.GetSize() - 1].trailingSpace = true;
						setNextSpace = false;
					} else {
						result.PushBack(t);
						setNextSpace = true;
					}

					break;
				}
			}
		}
		
	}

	result.Remove(0, 0);

	AnalyzeStrings(result);

	return std::move(result);
}


void Compiler::AnalyzeStrings(List<Token>& tokens) {

	while (true) {
		auto [indexStart, itemStart] = tokens.FindTuple(lang->syntax.stringStart, Token::CharCmp);

		if (indexStart == -1) break;

		int64 i = indexStart + 1;
		int64 numTokens = tokens.GetSize();

		itemStart.string = "";
		itemStart.isString = true;

		for (; i < numTokens; i++) {
			Token tmp = tokens[i];

			if (tmp.string[0] == lang->syntax.stringEnd)
				break;

			AnalyzeEscapeSequences(tmp);

			itemStart.string += tmp.string;
		}

		if (i >= numTokens) {
			Compiler::Log(itemStart, HC_ERROR_SYNTAX_MISSING_STRING_CLOSE, lang->syntax.stringEnd);
		}

		tokens.Remove(indexStart + 1, i);
	}
}

uint64 GetNumDigits(String& string, uint8 base, uint64 index) {
	uint64 count = 0;

	char c = string[index];

	while (index < string.length) {
		switch (base) {
			case 2:
				count += c >= '0' && c <= '1';
				break;
			case 8:
				count += c >= '0' && c <= '7';
				break;
			case 10:
				count += c >= '0' && c <= '9';
				break;
			case 16:
				count += (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
				break;
			default:
				break;
		}

		index++;
	}


	return count;
}

void Compiler::AnalyzeEscapeSequences(Token& token) {
	String& string = token.string;
	uint64 index = 0;

	while ((index = string.Find('\\', index)) != String::npos) {
		char sig = string[index + 1];
		for (uint64 i = 0; i < lang->syntax.numSequences; i++) {
			Syntax::EscapeSequence es = lang->syntax.escSequence[i];

			if (sig == es.signature) {
				if (es.base) {
					uint64 numDigits = GetNumDigits(string, es.base, index + 2);

					if (numDigits == 0) {
						Compiler::Log(token, HC_ERROR_SYNTAX_INT_LITERAL_NO_DIGIT, index + 2);
					}

					uint64 value = StringUtils::ToUint64(string.str, es.base, index + 2, index + 2 + numDigits - 1);

					if (value > 255) {
						Compiler::Log(token, HC_ERROR_SYNTAX_INT_LITERAL_TO_BIG, index + 2, value);
					}

					string.Replace(index, index + 1 + numDigits, (char)value);
				} else {
					string.Replace(index, index + 1, (char)es.value);
				}

				return;
			}
		}

		Compiler::Log(token, HC_ERROR_SYNTAX_INVALID_ESCAPE_CHARACTER, index, sig);
	}


}