#include <core/compiler/syntax.h>
#include <core/log/log.h>
#include <util/list.h>
#include <util/file.h>
#include <core/error/error.h>
#include <core/preprocessor/preprocessor.h>

#include <chrono>

int main(int argc, char** argv) {
	Syntax syntax;

	syntax.delimiters = " #=+-*/<>.,^&|(){}[]%\"'!?:;";
	syntax.stringStart = '"';
	syntax.stringEnd = '"';
	syntax.numSequences = 2;
	syntax.sequence = new EscapeSequence[3];
	syntax.sequence[0].signature = 'n';
	syntax.sequence[0].value = '\n';
	syntax.sequence[1].signature = 'x';
	syntax.sequence[1].base = 16;
	
	auto res = Lexer::Analyze("test.c", syntax);

	List<String> includes;
	
	printf("%s\n", PreProcessor::Run(res, includes, syntax).str);

	//Syntax::Analyze(syntax, res);


}
