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

void Compiler::SyntaxAnalazys(List<Token>& tokens) {
	Pattern pattern;

	while (tokens.GetSize()) {
		PatternResult res;
		MatchAnyPattern(pattern, tokens, &res);
	}

}

void Compiler::MatchAnyPattern(const Pattern& pattern, List<Token>& tokens, PatternResult* result) {
	List<Pattern::BasePattern*> patterns = { (Pattern::BasePattern*)&pattern.variablePattern };

	for (uint64 i = 0; i < patterns.GetSize(); i++) {
		MatchPattern(patterns[i], tokens, result);

		if (result->type != PatternType::Unknown) return;
	}
	
}

void Compiler::MatchPattern(const Pattern::BasePattern* pattern, List<Token>& tokens, PatternResult* result) {
	uint8 count = 0;
	
	if (pattern->type == PatternType::Variable) {
		const Pattern::BasePattern& p = *pattern;
		const List<Pattern::PatternItem>* next = &p.first;

		while (true) {
			const List<Pattern::PatternItem>& list = *next;

			if (list.GetSize() == 0) break;

			for (uint64 i = 0; i < list.GetSize(); i++) {
				const Pattern::PatternItem& item = list[i];

				bool match = false;
				TokenType actual = tokens[count].type;
				TokenType tok;

				//Check if any of the tokens match
				for (uint64 j = 0; j < item.tokens.GetSize(); j++) {
					tok = item.tokens[j];

					switch (tok) {
						case TokenType::Operators: {
							int32 operators = (int32)TokenType::Operators;
							int32 t = (int32)actual;

							match = (t > operators && t < operators + 100);
							break;
						}
						case TokenType::MiscTokens:	{
							int32 misc = (int32)TokenType::MiscTokens;
							int32 t = (int32)actual;

							match = (t > misc && t < misc + 100);
							break;
						}
						case TokenType::Keyword:
							for (uint64 c = 0; c < item.keywords.GetSize(); c++) {
								if (tokens[count].keyword == item.keywords[c]) {
									match = true;
									break;
								}
							}

							break;
						default:
							match = actual == tok;
					}

					if (match) break;
				}

				if (!match) continue;

				result->items.PushBack({ tok, tokens[count++] });
				next = &item.next;
				break;
			}
		}

		if (result->items.GetSize() != 0) {
			result->type = PatternType::Variable;
		}
	}
}
