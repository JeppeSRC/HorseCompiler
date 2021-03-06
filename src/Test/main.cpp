#include <core/log/log.h>
#include <util/list.h>
#include <util/file.h>
#include <core/error/error.h>
#include <core/preprocessor/preprocessor.h>
#include <core/compiler/compiler.h>

#include <chrono>
#include <Windows.h>

int main(int argc, char** argv) {
	char buf[1024];

	GetCurrentDirectoryA(1024, buf);

	Language lang;

	Compiler compiler(String(buf), &lang);

	lang.syntax.delimiters = " #=+-*/<>.,^&|(){}[]%\"'!?:;";
	lang.syntax.stringStart = '"';
	lang.syntax.stringEnd = '"';
	lang.syntax.charStart = '\'';
	lang.syntax.charEnd = '\'';
	lang.syntax.numSequences = 2;
	lang.syntax.escSequence = new Syntax::EscapeSequence[3];
	lang.syntax.escSequence[0].signature = 'n';
	lang.syntax.escSequence[0].value = '\n';
	lang.syntax.escSequence[1].signature = 'x';
	lang.syntax.escSequence[1].base = 16;

	List<TokenTypeDef>& type = lang.syntax.tokenTypes;

	type.PushBack({ TokenType::OpAssign, "=" });
	type.PushBack({ TokenType::Plus, "+" });
	type.PushBack({ TokenType::Minus, "-" });
	type.PushBack({ TokenType::Asterix, "*" });
	type.PushBack({ TokenType::Slash, "/" });
	type.PushBack({ TokenType::OpCompoundAdd, "+=" });
	type.PushBack({ TokenType::OpCompoundSub, "-=" });
	type.PushBack({ TokenType::OpCompoundMul, "*=" });
	type.PushBack({ TokenType::OpCompoundDiv, "/=" });

	type.PushBack({ TokenType::OpBitNot, "~" });
	type.PushBack({ TokenType::Ampersand, "&" });
	type.PushBack({ TokenType::OpBitOr, "|" });
	type.PushBack({ TokenType::OpLeftShift, "<<" });
	type.PushBack({ TokenType::OpRightShift, ">>" });

	type.PushBack({ TokenType::OpNot, "!" });
	type.PushBack({ TokenType::OpAnd, "&&" });
	type.PushBack({ TokenType::OpOr, "||" });
	type.PushBack({ TokenType::OpEqual, "==" });
	type.PushBack({ TokenType::OpNotEqual, "!=" });

	type.PushBack({ TokenType::OpInc, "++" });
	type.PushBack({ TokenType::OpDec, "--" });

	type.PushBack({ TokenType::OpLess, "<" });
	type.PushBack({ TokenType::OpGreater, ">" });
	type.PushBack({ TokenType::OpLessEq, "<=" });
	type.PushBack({ TokenType::OpGreaterEq, ">=" });

	type.PushBack({ TokenType::Dot, "." });
	type.PushBack({ TokenType::Comma, "," });
	type.PushBack({ TokenType::Semicolon, ";" });
	type.PushBack({ TokenType::Colon, ":" });

	type.PushBack({ TokenType::Questionmark, "?" });

	type.PushBack({ TokenType::ParenthesisOpen, "(" });
	type.PushBack({ TokenType::ParenthesisClose, ")" });
	type.PushBack({ TokenType::BracketOpen, "{" });
	type.PushBack({ TokenType::BracketClose, "}" });
	type.PushBack({ TokenType::SqBracketOpen, "[" });
	type.PushBack({ TokenType::SqBracketClose, "]" });
	type.PushBack({ TokenType::Semicolon, ";" });

	auto& primitiveTypes = lang.primitiveTypes;

	primitiveTypes.PushBack({ PrimitiveType::Void, "void" });
	primitiveTypes.PushBack({ PrimitiveType::Char, "char" });
	primitiveTypes.PushBack({ PrimitiveType::Short, "short" });
	primitiveTypes.PushBack({ PrimitiveType::Int, "int" });
//	primitiveTypes.PushBack({ PrimitiveType::Long, "long" });
	primitiveTypes.PushBack({ PrimitiveType::Float, "float" });
//	primitiveTypes.PushBack({ PrimitiveType::Double, "double" });
	primitiveTypes.PushBack({ PrimitiveType::Vec2, "vec2" });
	primitiveTypes.PushBack({ PrimitiveType::Vec3, "vec3" });
	primitiveTypes.PushBack({ PrimitiveType::Vec4, "vec4" });
	primitiveTypes.PushBack({ PrimitiveType::Mat4, "mat4" });
	primitiveTypes.PushBack({ PrimitiveType::Const, "const" });
	primitiveTypes.PushBack({ PrimitiveType::Signed, "signed" });
	primitiveTypes.PushBack({ PrimitiveType::Unsigned, "unsigned" });


	auto& keywords = lang.keywords;

	keywords.PushBack({ KeywordType::Typedef, "typedef" });

	keywords.PushBack({ KeywordType::If, "if" });
	keywords.PushBack({ KeywordType::For, "for" });
	keywords.PushBack({ KeywordType::While, "while" });
	keywords.PushBack({ KeywordType::Switch, "switch" });

	keywords.PushBack({ KeywordType::Struct, "struct" });

	keywords.PushBack({ KeywordType::Extern, "extern" });

	keywords.PushBack({KeywordType::Layout, "layout" });
	keywords.PushBack({KeywordType::In, "in" });
	keywords.PushBack({KeywordType::Out, "out" });

	auto res = compiler.LexicalAnalazys("test.c");

	List<String> includes;

	PreProcessor pp(includes, &compiler);

	String s = pp.Run(res);

	printf("%s\n", s.str);

	compiler.SyntaxAnalazys(res);
}
