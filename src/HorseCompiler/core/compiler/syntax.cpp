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

#include "syntax.h"
#include "compiler.h"

#include <util/util.h>

void Syntax::Analyze(Lexer::AnalysisResult& lexerResult) {
	Syntax* syntax = Compiler::GetSyntax();
	AnalyzeStrings(syntax, lexerResult);
}

void Syntax::AnalyzeStrings(const Syntax* syntax, Lexer::AnalysisResult& lexerResult) {

	while (true) {
		auto [indexStart, itemStart] = lexerResult.tokens.FindTuple(syntax->stringStart, Token::CharCmp);

		if (indexStart == -1) break;

		int64 i = indexStart + 1;
		int64 numTokens = lexerResult.tokens.GetSize();

		itemStart.string = "";
		itemStart.isString = true;

		for (; i < numTokens; i++) {
			Token tmp = lexerResult.tokens[i];

			if (tmp.string[0] == syntax->stringEnd)
				break;
				
			AnalyzeEscapeSequences(syntax, tmp);

			itemStart.string += tmp.string + " ";
		}

		if (i >= numTokens) {
			Compiler::Log(itemStart, HC_ERROR_SYNTAX_MISSING_STRING_CLOSE);
		}

		itemStart.string.RemoveAt(itemStart.string.length - 1);
		lexerResult.tokens.Remove(indexStart + 1, i);
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

void Syntax::AnalyzeEscapeSequences(const Syntax* syntax, Token& token) {
	String& string = token.string;
	uint64 index = 0;

	while ((index = string.Find('\\', index)) != String::npos) {
		char sig = string[index + 1];
		for (uint64 i = 0; i < syntax->numSequences; i++) {
			EscapeSequence es = syntax->sequence[i];

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

