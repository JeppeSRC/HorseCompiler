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

	List<KeywordType> types2 = { KeywordType::Char, KeywordType::Short, KeywordType::Int, KeywordType::Long };

	PatternItem cnst(TokenType::Keyword, KeywordType::Const);
	PatternItem type({ TokenType::Keyword, TokenType::Identifier }, types);
	PatternItem type2(TokenType::Keyword, types2);
	PatternItem sign(TokenType::Keyword, { KeywordType::Signed, KeywordType::Unsigned }, type2);

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
		auto t2 = type2;
		auto c = cnst;
		auto s = sign;

		t.next.PushBack(name);
		t2.next.PushBack(name);

		c.next = { s, t };
		first.PushBack(c);
	}

	//<sign> <type> <name>
	//<sign> <type> <const> <name>
	{
		auto t2 = type2;
		auto c = cnst;
		auto s = sign;

		c.next.PushBack(name);
		t2.next = { name, c };

		first.PushBack(s);
	}


}

Pattern::FunctionDeclarationPattern::FunctionDeclarationPattern() : BasePattern(PatternType::Function) {
	List<TokenType> tmp{ TokenType::Semicolon, TokenType::BracketOpen };
	PatternItem colonOrBody(tmp);
	PatternItem close(TokenType::ParenthesisClose, colonOrBody);
	PatternItem open(TokenType::ParenthesisOpen, close);
	PatternItem name(TokenType::Identifier, open);

	List<KeywordType> types = { KeywordType::Char, KeywordType::Short, KeywordType::Int,
								KeywordType::Long, KeywordType::Float, KeywordType::Double,
								KeywordType::Vec2, KeywordType::Vec3, KeywordType::Vec4,
								KeywordType::Mat4, KeywordType::Void };

	List<KeywordType> types2 = { KeywordType::Char, KeywordType::Short, KeywordType::Int, KeywordType::Long };

	PatternItem cnst(TokenType::Keyword, KeywordType::Const);
	PatternItem type({ TokenType::Keyword, TokenType::Identifier }, types);
	PatternItem type2(TokenType::Keyword, types2);
	PatternItem sign(TokenType::Keyword, { KeywordType::Signed, KeywordType::Unsigned }, type2);

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
		auto t2 = type2;
		auto c = cnst;
		auto s = sign;

		t.next.PushBack(name);
		t2.next.PushBack(name);

		c.next = { s, t };
		first.PushBack(c);
	}

	//<sign> <type> <name>
	//<sign> <type> <const> <name>
	{
		auto t2 = type2;
		auto c = cnst;
		auto s = sign;

		c.next.PushBack(name);
		t2.next = { name, c };

		first.PushBack(s);
	}
}

Pattern::ParamPattern::ParamPattern() : BasePattern(PatternType::Function) {
	List<TokenType> tmp({ TokenType::Comma, TokenType::ParenthesisClose });
	PatternItem end(tmp);
	PatternItem name(TokenType::Identifier, end);
	PatternItem ref(TokenType::Ampersand, name);

	List<KeywordType> types = { KeywordType::Char, KeywordType::Short, KeywordType::Int,
								KeywordType::Long, KeywordType::Float, KeywordType::Double,
								KeywordType::Vec2, KeywordType::Vec3, KeywordType::Vec4,
								KeywordType::Mat4 };

	List<KeywordType> types2 = { KeywordType::Char, KeywordType::Short, KeywordType::Int, KeywordType::Long };

	PatternItem cnst(TokenType::Keyword, KeywordType::Const);
	PatternItem type({ TokenType::Keyword, TokenType::Identifier }, types);
	PatternItem type2(TokenType::Keyword, types2);
	PatternItem sign(TokenType::Keyword, { KeywordType::Signed, KeywordType::Unsigned }, type2);

	//<type> <name>
	//<type> <const> <name>
	//<type> <ref> <name>
	//<type> <const> <ref> <name>
	{
		auto t = type;
		auto c = cnst;

		c.next = { name, ref };

		t.next = { name, c , ref };

		first.PushBack(t);
	}

	//<const> <type> <name>
	//<const> <sign> <type> <name>
	//<const> <type> <ref> <name>
	//<const> <sign> <type> <ref> <name>
	{
		auto t = type;
		auto t2 = type2;
		auto c = cnst;
		auto s = sign;

		t.next = { name , ref };
		t2.next = { name , ref };

		c.next = { s, t };
		first.PushBack(c);
	}

	//<sign> <type> <name>
	//<sign> <type> <const> <name>
	//<sign> <type> <ref> <name>
	//<sign> <type> <const> <ref> <name>
	{
		auto t2 = type2;
		auto c = cnst;
		auto s = sign;

		c.next = { name , ref };
		t2.next = { name, c, ref };

		first.PushBack(s);
	}

}
