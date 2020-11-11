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
#include <stdarg.h>



static Syntax syntax;

String Compiler::currentDir = "<NotSet>";

void Compiler::SetSyntax(Syntax newSyntax) {
	syntax = newSyntax;
}

Syntax* Compiler::GetSyntax() {
	return &syntax;
}

const String& Compiler::GetCurrentDir() {
	return currentDir;
}

void Compiler::SetCurrentDir(const String& cwd) {
	currentDir = cwd;
}

void Compiler::Log(const Token& item, uint64 code, ...) {
	va_list list;
	va_start(list, code);

	switch (code) {
		case HC_ERROR_SYNTAX_MISSING_STRING_CLOSE:
			Log::Error(item.line, item.column, item.filename.str, code, "missing closing string character '%c'", syntax.stringEnd);
			break;
		case HC_ERROR_SYNTAX_INVALID_ESCAPE_CHARACTER:
			Log::Warning(item.line, item.column + va_arg(list, uint64), item.filename.str, code, "unrecognized escape character '%c' sequence", va_arg(list, char));
			break;
		case HC_ERROR_SYNTAX_INT_LITERAL_NO_DIGIT:
			Log::Error(item.line, item.column + va_arg(list, uint64), item.filename.str, code, "integer literal must have at least one digit");
			break;
		case HC_ERROR_SYNTAX_INT_LITERAL_TO_BIG:
			Log::Error(item.line, item.column + va_arg(list, uint64), item.filename.str, code, "integer literal to big for a character '%u'", va_arg(list, uint64));
			break;
		case HC_ERROR_PREPROCESSOR_NO_DIRECTIVE:
			Log::Error(item.line, item.column, item.filename.str, code, "no directive");
			break;
		case HC_ERROR_PREPROCESSOR_UNKNOWN_DIRECTIVE:
			Log::Error(item.line, item.column, item.filename.str, code, "unknown preprocessor directive \"%s\"", va_arg(list, char*));
			break;
		case HC_ERROR_PREPROCESSOR_INCLUDE_FILE_NOT_FOUND:
			Log::Error(item.line, item.column, item.filename.str, code, "no such file or directory \"%s\"", va_arg(list, char*));
			break;
		case HC_ERROR_PREPROCESSOR_INCLUDE_UNKNOWN_SYMBOL1:
			Log::Error(item.line, item.column, item.filename.str, code, "unkown symbol in include directive '%c', expected '\"' or '<'", va_arg(list, char));
			break;
		case HC_ERROR_PREPROCESSOR_INCLUDE_UNKNOWN_SYMBOL2:
			Log::Error(item.line, item.column, item.filename.str, code, "unkown symbol in include directive '%c', expected '%c'", va_arg(list, char), va_arg(list, char));
			break;
	}
}