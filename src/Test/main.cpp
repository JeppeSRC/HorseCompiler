#include <core/compiler/syntax.h>
#include <core/log/log.h>
#include <util/list.h>
#include <util/file.h>
#include <core/error/error.h>
#include <core/preprocessor/preprocessor.h>
#include <core/compiler/compiler.h>

#include <chrono>
#include <Windows.h>

int main(int argc, char** argv) {
	Syntax* syntax = Compiler::GetSyntax();

	syntax->delimiters = " #=+-*/<>.,^&|(){}[]%\"'!?:;";
	syntax->stringStart = '"';
	syntax->stringEnd = '"';
	syntax->numSequences = 2;
	syntax->sequence = new EscapeSequence[3];
	syntax->sequence[0].signature = 'n';
	syntax->sequence[0].value = '\n';
	syntax->sequence[1].signature = 'x';
	syntax->sequence[1].base = 16;

	auto res = Lexer::Analyze("test.c");

	List<String> includes;
	
	printf("%s\n", PreProcessor::Run(res, includes).str);

	//Syntax::Analyze(syntax, res);


}
