/*
MIT License

Copyright (c) 2021 Jesper

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
#include <core/compiler/compiler.h>

#include <util/file.h>
#include <util/util.h>

#include <algorithm>

#define IN_STRING 0x01
#define IN_INCLUDE 0x02
#define IN_CHAR 0x03

Tokens Lexer::Analyze(const String& filename, Language* lang) {
	Lexer lex(lang);

	return lex.Analyze(filename);
}

Tokens Lexer::Analyze(const String& filename) {
	Tokens result;
	List<uint64> indices;
	List<uint64> newLines;

	SourceFile* sourceFile = new SourceFile(filename);
	String& file = sourceFile->text;

	result.Reserve(4096);
	indices.Reserve(4096);
	newLines.Reserve(4096);

	result.PushBack(Token());

	uint64 index = 0;

	for (uint64 i = 0; i < lang->delimiters.length; i++) {
		index = 0;

		while ((index = file.Find(lang->delimiters[i], index)) != String::npos) {
			indices.PushBack(index++);
		}
	}

	index = 0;

	while ((index = file.Find('\n', index)) != String::npos) {
		indices.PushBack(index);
		newLines.PushBack(index++);
	}

	newLines.PushBack(-1);
	indices.PushBack(-1);

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
					t.loc = SourceLocation(sourceFile, lastIndex, currLine + 1, lastIndex - ((newLines[currLine - 1 * (currLine > 0)] + 1) * (currLine > 0)) + 1);
					t.string = file.SubString(lastIndex, i - 1);
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
					t.loc = SourceLocation(sourceFile, i, currLine + 1, i - ((newLines[currLine - 1 * (currLine > 0)] + 1) * (currLine > 0)) + 1);
					t.string = c;
					t.isString = (bool)includeSpaces;

					if (c == lang->charStart) {
						if (includeSpaces == 0) {
							includeSpaces = IN_CHAR;
						} else if (includeSpaces == IN_CHAR) {
							includeSpaces = 0;
						}
					} else if (c == lang->charEnd) {
						if (includeSpaces == IN_CHAR) {
							includeSpaces = 0;
						}
					}

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

	result.Remove(0);

	ParseStrings(result);


	//TokenTypes
	for (uint64 i = 0; i < result.GetSize(); i++) {
		Token& token = result[i];



		if (token.isString) continue;

		for (uint64 j = 0; j < lang->tokenTypes.GetSize(); j++) {
			const TokenTypeDef& def = lang->tokenTypes[j];
			uint64 len = def.def.length;

			if (len == 1 && token.string[0] == def.def[0]) {
				token.type = def.type;
			} else {
				bool match = true;

				for (uint64 k = 0; k < len; k++) {
					if (def.def[k] != result[i + k].string[0] || result[i].trailingSpace) {
						match = false;
						break;
					}
				}

				if (match) {
					token.type = def.type;
					token.string = def.def;
					result.Remove(i + 1, i + len);
				}
			}
		}

		if (token.type == TokenType::Unknown) {
			if ((token.string[0] >= '0' && token.string[0] <= '9') ) {
				token.type = TokenType::Literal;
			} else {
				token.type = TokenType::Identifier;
			}
		}
	}

	//Keywords
	for (uint64 i = 0; i < result.GetSize(); i++) {
		Token& token = result[i];

		if (token.isString || token.type != TokenType::Identifier) continue;

		for (uint64 j = 0; j < lang->keywords.GetSize(); j++) {
			const KeywordDef& def = lang->keywords[j];

			if (token.string == def.def) {
				token.type = TokenType::Keyword;
				token.keyword = def.keyword;
				break;
			}
		}
	}

	//Primitives
	for (uint64 i = 0; i < result.GetSize(); i++) {
		Token& token = result[i];

		if (token.isString || token.type != TokenType::Identifier) continue;

		for (uint64 j = 0; j < lang->primitiveTypes.GetSize(); j++) {
			const PrimitiveTypeDef& def = lang->primitiveTypes[j];

			if (token.string == def.def) {
				token.type = TokenType::PrimitiveType;
				token.primitiveType = def.type;
				break;
			}
		}
	}

	OperatorType prevType = OperatorType::Unknown;

	//Operators
	for (uint64 i = 0; i < result.GetSize(); i++) {
		Token& token = result[i];

		if (token.isString || token.type != TokenType::Identifier) continue;

		for (uint64 j = 0; j < lang->operators.GetSize(); j++) {
			const OperatorTypeDef& def = lang->operators[j];

			if (token.string == def.def) {
				token.type = TokenType::Operator;
				token.operatorType = def.type;

				if ((def.type == OperatorType::OpAdd || def.type == OperatorType::OpSub || def.type == OperatorType::OpMul || def.type == OperatorType::OpDiv) && token.trailingSpace == false && prevType == OperatorType::Unknown) {
					prevType = def.type;
				} else if ((def.type == OperatorType::OpAdd || def.type == OperatorType::OpSub || def.type == OperatorType::OpAssign) && prevType != OperatorType::Unknown) {
					Token& prevToken = result[i - 1];

					switch (def.type) {
						case OperatorType::OpAdd:
							prevToken.operatorType = OperatorType::OpInc;
							prevToken.string += "+";
							break;
						case OperatorType::OpSub:
							prevToken.operatorType = OperatorType::OpDec;
							prevToken.string += "-";
							break;
						case OperatorType::OpAssign:
							prevToken.string += "=";

							switch (prevType) {
								case OperatorType::OpAdd:
									prevToken.operatorType = OperatorType::OpCompoundAdd;
									break;
								case OperatorType::OpSub:
									prevToken.operatorType = OperatorType::OpCompoundSub;
									break;
								case OperatorType::OpMul:
									prevToken.operatorType = OperatorType::OpCompoundMul;
									break;
								case OperatorType::OpDiv:
									prevToken.operatorType = OperatorType::OpCompoundDiv;
									break;
							}
							break;
					}

					result.Remove(i--);

					prevType = OperatorType::Unknown;
				} else {
					prevType = OperatorType::Unknown;
				}

				if (def.type == OperatorType::OpSub && i > 0) {
					if (result[i-1].type == TokenType::Operator) {
						token.operatorType = OperatorType::OpNegate;
					}
				}

				break;
			}
		}
	}

	for (uint64 i = 0; i < result.GetSize(); i++) {
		Token& token = result[i];

		if (token.type == TokenType::Literal) {
			ParseLiteral(result, i);
		}
	}

	return std::move(result);
}


void Lexer::ParseLiteral(Tokens& tokens, uint64 index) {
	Token& token = tokens[index++];

	if (index >= tokens.GetSize() - 1) {
		Compiler::Log(token, HC_ERROR_LEXER_EOL);
		return;
	}

	Token& dot = tokens[index];

	if (dot.type == TokenType::Operator && dot.operatorType == OperatorType::Dot) {
		token.primitiveType = PrimitiveType::Float;
		token.string += ".";

		tokens.Remove(index);

		if (index >= tokens.GetSize() - 1) {
			Compiler::Log(token, HC_ERROR_LEXER_EOL);
			return;
		}

		Token& next = tokens[index];

		if (next.type == TokenType::Literal || (next.string.length == 1 && next.string[0] == 'f')) {
			token.string += next.string;
			tokens.Remove(index);
		}
	} else {
		token.primitiveType = PrimitiveType::Int;
	}
}

void Lexer::ParseStrings(Tokens& tokens) {

	while (true) {
		auto [indexStart, itemStart] = tokens.FindTuple(lang->stringStart, Token::CharCmp);

		if (indexStart == -1) break;

		int64 i = indexStart + 1;
		int64 numTokens = tokens.GetSize();

		itemStart.string = "";
		itemStart.isString = true;

		for (; i < numTokens; i++) {
			Token tmp = tokens[i];

			if (tmp.string[0] == lang->stringEnd)
				break;

			ParseEscapeSequences(tmp);

			itemStart.string += tmp.string;
		}

		if (i >= numTokens) {
			Compiler::Log(itemStart, HC_ERROR_SYNTAX_MISSING_STRING_CLOSE, lang->stringEnd);
		}

		itemStart.type = TokenType::Literal;

		tokens.Remove(indexStart + 1, i);
	}

	while (true) {
		auto [indexStart, itemStart] = tokens.FindTuple(lang->charStart, Token::CharCmp);

		if (indexStart == -1) break;

		uint64 end = tokens.Find(lang->charEnd, Token::CharCmp, indexStart+1);
		uint64 len = end - (indexStart + 1);

		String tmp("");

		if (len > 1) {
			Compiler::Log(itemStart, HC_ERROR_SYNTAX_CHAR_LITERAL_TO_MANY_CHARS);
		} else if (len == 1) {
			tmp.Append(tokens[indexStart + 1].string);
		}

		itemStart.string = tmp;
		itemStart.type = TokenType::Literal;
		itemStart.primitiveType = PrimitiveType::Byte;

		tokens.Remove(indexStart + 1, end);
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

void Lexer::ParseEscapeSequences(Token& token) {
	String& string = token.string;
	uint64 index = 0;

	while ((index = string.Find('\\', index)) != String::npos) {
		char sig = string[index + 1];
		for (uint64 i = 0; i < lang->numSequences; i++) {
			Language::EscapeSequence es = lang->escSequence[i];

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

		Compiler::Log(token, HC_WARN_SYNTAX_INVALID_ESCAPE_CHARACTER, index, sig);
	}


}