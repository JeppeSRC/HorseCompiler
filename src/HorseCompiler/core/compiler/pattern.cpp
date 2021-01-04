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


Pattern::VariablePattern::VariablePattern() : BasePattern(PatternType::Variable) {
	List<TokenType> tmp({ TokenType::OpAssign, TokenType::Semicolon });
	PatternItem end(tmp);
	PatternItem name(TokenType::Identifier, end);

	List<KeywordType> types = { KeywordType::Char, KeywordType::Short, KeywordType::Int,
								KeywordType::Long, KeywordType::Float, KeywordType::Double,
								KeywordType::Vec2, KeywordType::Vec3, KeywordType::Vec4,
								KeywordType::Mat4 };

	PatternItem cnst(TokenType::Keyword, KeywordType::Const);
	PatternItem type({ TokenType::Keyword, TokenType::Identifier }, types);
	PatternItem sign(TokenType::Keyword, { KeywordType::Signed, KeywordType::Unsigned }, type);

	//<type> <name>
	//<type> <const> <name>
	{
		auto t = type;
		auto c = cnst;

		c.next.PushBack(name);

		t.next = { name, c };

		first.PushBack(t);
	}

	//<const> <type> <name>
	//<const> <sign> <type> <name>
	{
		auto t = type;
		auto c = cnst;
		auto s = sign;

		t.next.PushBack(name);
		s.next.PushBack(t);

		c.next = { s, t };
		first.PushBack(c);
	}

	//<sign> <type> <name>
	//<sign> <type> <const> <name>
	{
		auto t = type;
		auto c = cnst;
		auto s = sign;

		c.next.PushBack(name);
		t.next = { name, c };
		s.next.PushBack(t);

		first.PushBack(s);
	}


}

}
