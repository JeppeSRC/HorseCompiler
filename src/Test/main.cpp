#include <core/log/log.h>
#include <util/list.h>
#include <util/file.h>
#include <core/error/error.h>
#include <core/preprocessor/preprocessor.h>
#include <core/compiler/compiler.h>
#include <core/compiler/lexer/lexer.h>
#include <core/compiler/parsing/syntax.h>
#include <core/compiler/semantic/semantic.h>

#include <chrono>
#include <Windows.h>

int main(int argc, char** argv) {
	char buf[1024];

	GetCurrentDirectoryA(1024, buf);

	Compiler compiler(String(buf), Language::Default());

	auto res = Lexer::Analyze("test.c", Language::Default());

	List< String > includes;

	PreProcessor pp(includes, &compiler);

	if (!pp.Run(res)) {
		return 1;
	}

	SymbolTable symbols;
	TypeTable types;
	ASTNode* rootNode = new ASTNode(ASTType::Root);


	Syntax::Analyze(res, 0, rootNode, Language::Default());
	Semantic::Analyze(rootNode, &types, &symbols);
}
