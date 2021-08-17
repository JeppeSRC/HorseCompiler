#include <core/log/log.h>
#include <util/list.h>
#include <util/file.h>
#include <core/error/error.h>
#include <core/preprocessor/preprocessor.h>
#include <core/compiler/compiler.h>
#include <core/compiler/ast.h>

#include <chrono>
#include <Windows.h>

int main(int argc, char** argv) {
	char buf[1024];

	GetCurrentDirectoryA(1024, buf);

	Language lang;

	Compiler compiler(String(buf), &lang);

	lang.syntax.delimiters               = " #=+-*/<>.,^&|(){}[]%\"'!?:;";
	lang.syntax.stringStart              = '"';
	lang.syntax.stringEnd                = '"';
	lang.syntax.charStart                = '\'';
	lang.syntax.charEnd                  = '\'';
	lang.syntax.numSequences             = 2;
	lang.syntax.escSequence              = new Syntax::EscapeSequence[3];
	lang.syntax.escSequence[0].signature = 'n';
	lang.syntax.escSequence[0].value     = '\n';
	lang.syntax.escSequence[1].signature = 'x';
	lang.syntax.escSequence[1].base      = 16;

	auto& operators = lang.operators;

	operators.PushBack({ OperatorType::Dot, ".", OperatorAssociativity::LTR, 1 });
	operators.PushBack({ OperatorType::OpInc, "++", OperatorAssociativity::LTR, 1, OperandType::Any, OperandType::None });
	operators.PushBack({ OperatorType::OpDec, "--", OperatorAssociativity::LTR, 1, OperandType::Any, OperandType::None });
	operators.PushBack({ OperatorType::OpSqBracketOpen, "[", OperatorAssociativity::LTR, 1 });
	operators.PushBack({ OperatorType::OpSqBracketClose, "]", OperatorAssociativity::LTR, 1 });

	operators.PushBack({ OperatorType::OpInc, "++", OperatorAssociativity::RTL, 2, OperandType::None, OperandType::Any });
	operators.PushBack({ OperatorType::OpDec, "--", OperatorAssociativity::RTL, 2, OperandType::None, OperandType::Any });
	operators.PushBack({ OperatorType::OpBitNot, "~", OperatorAssociativity::RTL, 2 });
	operators.PushBack({ OperatorType::OpNot, "!", OperatorAssociativity::RTL, 2 });

	operators.PushBack({ OperatorType::OpMul, "*", OperatorAssociativity::LTR, 3 });
	operators.PushBack({ OperatorType::OpDiv, "/", OperatorAssociativity::LTR, 3 });

	operators.PushBack({ OperatorType::OpAdd, "+", OperatorAssociativity::LTR, 4 });
	operators.PushBack({ OperatorType::OpSub, "-", OperatorAssociativity::LTR, 4 });

	operators.PushBack({ OperatorType::OpLeftShift, "<<", OperatorAssociativity::LTR, 5 });
	operators.PushBack({ OperatorType::OpRightShift, ">>", OperatorAssociativity::LTR, 5 });

	operators.PushBack({ OperatorType::OpLess, "<", OperatorAssociativity::LTR, 6 });
	operators.PushBack({ OperatorType::OpGreater, ">", OperatorAssociativity::LTR, 6 });
	operators.PushBack({ OperatorType::OpLessEq, "<=", OperatorAssociativity::LTR, 6 });
	operators.PushBack({ OperatorType::OpGreaterEq, ">=", OperatorAssociativity::LTR, 6 });

	operators.PushBack({ OperatorType::OpEqual, "==", OperatorAssociativity::LTR, 7 });
	operators.PushBack({ OperatorType::OpNotEqual, "!=", OperatorAssociativity::LTR, 7 });

	operators.PushBack({ OperatorType::OpBitAnd, "&", OperatorAssociativity::LTR, 8 });

	operators.PushBack({ OperatorType::OpBitXor, "^", OperatorAssociativity::LTR, 9 });

	operators.PushBack({ OperatorType::OpBitOr, "|", OperatorAssociativity::LTR, 10 });

	operators.PushBack({ OperatorType::OpAnd, "&&", OperatorAssociativity::LTR, 11 });

	operators.PushBack({ OperatorType::OpOr, "||", OperatorAssociativity::LTR, 12 });

	operators.PushBack({ OperatorType::OpAssign, "=", OperatorAssociativity::RTL, 14, OperandType::Any });
	operators.PushBack({ OperatorType::OpCompoundAdd, "+=", OperatorAssociativity::RTL, 14, OperandType::Any });
	operators.PushBack({ OperatorType::OpCompoundSub, "-=", OperatorAssociativity::RTL, 14, OperandType::Any });
	operators.PushBack({ OperatorType::OpCompoundMul, "*=", OperatorAssociativity::RTL, 14, OperandType::Any });
	operators.PushBack({ OperatorType::OpCompoundDiv, "/=", OperatorAssociativity::RTL, 14, OperandType::Any });

	List< TokenTypeDef >& type = lang.syntax.tokenTypes;

	type.PushBack({ TokenType::Comma, "," });
	type.PushBack({ TokenType::Semicolon, ";" });
	type.PushBack({ TokenType::Colon, ":" });

	type.PushBack({ TokenType::Questionmark, "?" });

	type.PushBack({ TokenType::ParenthesisOpen, "(" });
	type.PushBack({ TokenType::ParenthesisClose, ")" });
	type.PushBack({ TokenType::BracketOpen, "{" });
	type.PushBack({ TokenType::BracketClose, "}" });

	type.PushBack({ TokenType::Semicolon, ";" });

	auto& primitiveTypes = lang.primitiveTypes;

	primitiveTypes.PushBack({ PrimitiveType::Void, "void" });
	primitiveTypes.PushBack({ PrimitiveType::Byte, "byte" });
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
	keywords.PushBack({ KeywordType::Else, "else" });
	keywords.PushBack({ KeywordType::For, "for" });
	keywords.PushBack({ KeywordType::While, "while" });
	keywords.PushBack({ KeywordType::Switch, "switch" });
	keywords.PushBack({ KeywordType::Return, "return" });

	keywords.PushBack({ KeywordType::Struct, "struct" });

	keywords.PushBack({ KeywordType::Extern, "extern" });

	keywords.PushBack({ KeywordType::Layout, "layout" });
	keywords.PushBack({ KeywordType::In, "in" });
	keywords.PushBack({ KeywordType::Out, "out" });
	keywords.PushBack({ KeywordType::Uniform, "uniform" });

	auto res = compiler.LexicalAnalazys("test.c");

	List< String > includes;

	PreProcessor pp(includes, &compiler);

	String s = pp.Run(res);

	printf("%s\n", s.str);

	ASTNode* rootNode = new ASTNode(ASTType::Root);

	compiler.SyntaxAnalazys(res, 0, rootNode);
}
