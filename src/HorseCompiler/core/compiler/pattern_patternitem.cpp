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

#include "pattern.h"

Pattern::PatternItem::PatternItem(TokenType token, const List<PatternItem>& next) : next(next) {
	tokens.PushBack(token);
}

Pattern::PatternItem::PatternItem(TokenType token, const PatternItem& next) {
	tokens.PushBack(token);
	if (next.tokens.GetSize()) this->next.PushBack(next);
}

Pattern::PatternItem::PatternItem(const List<TokenType>& tokens, const List<PatternItem>& next) : tokens(tokens), next(next) {}

Pattern::PatternItem::PatternItem(const List<TokenType>& tokens, const PatternItem& next) : tokens(tokens) {
	if (next.tokens.GetSize()) this->next.PushBack(next);
}



Pattern::PatternItem::PatternItem(TokenType token, KeywordType keyword, const List<PatternItem>& next) : next(next) {
	tokens.PushBack(token);
	keywords.PushBack(keyword);
}

Pattern::PatternItem::PatternItem(TokenType token, KeywordType keyword, const PatternItem& next) {
	tokens.PushBack(token);
	if (next.tokens.GetSize()) this->next.PushBack(next);
	keywords.PushBack(keyword);
}

Pattern::PatternItem::PatternItem(const List<TokenType>& tokens, KeywordType keyword, const List<PatternItem>& next) : tokens(tokens), next(next) {
	keywords.PushBack(keyword);
}

Pattern::PatternItem::PatternItem(const List<TokenType>& tokens, KeywordType keyword, const PatternItem& next) : tokens(tokens) {
	if (next.tokens.GetSize()) this->next.PushBack(next);
	keywords.PushBack(keyword);
}



Pattern::PatternItem::PatternItem(TokenType token, const List<KeywordType>& keyword, const List<PatternItem>& next) : keywords(keyword), next(next) {
	tokens.PushBack(token);
}

Pattern::PatternItem::PatternItem(TokenType token, const List<KeywordType>& keyword, const PatternItem& next) : keywords(keyword) {
	tokens.PushBack(token);
	if (next.tokens.GetSize()) this->next.PushBack(next);
}

Pattern::PatternItem::PatternItem(const List<TokenType>& tokens, const List<KeywordType>& keyword, const List<PatternItem>& next) : keywords(keyword), tokens(tokens), next(next) {
}

Pattern::PatternItem::PatternItem(const List<TokenType>& tokens, const List<KeywordType>& keyword, const PatternItem& next) : keywords(keyword), tokens(tokens) {
	if (next.tokens.GetSize()) this->next.PushBack(next);
}


Pattern::PatternItem::~PatternItem() {
	/*for (uint64 i = 0; i < next.GetSize(); i++) {
		delete next[i];
	}*/
}
