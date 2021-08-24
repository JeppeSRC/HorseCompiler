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

#include "compiler.h"

#include <util/util.h>
#include <stdarg.h>

Compiler::Compiler(const String& cwd, Language* language) : lang(language) {
	currentDir = cwd;
	StringUtils::ReplaceChar(currentDir, '\\', '/');

	if (!currentDir.EndsWith("/"))
		currentDir.Append("/");
}

/*
String Compiler::GetPrimitiveTypeString(const PrimitiveType& type) {
	for (uint64 i = 0; i < lang->primitiveTypes.GetSize(); i++) {
		auto& t = lang->primitiveTypes[i];

		if (t.type == type)
			return t.def;
	}

	return "";
}

Type* Compiler::GetType(const String& name) {
	for (uint64 i = 0; i < types.GetSize(); i++) {
		if (types[i]->name == name)
			return types[i];
	}

	return nullptr;
}

TypeScalar* Compiler::MakeTypeScalar(PrimitiveType type, uint8 sign, uint8 constness) {
	String name = constness ? "const " : "";

	// If sign == 2 it will be set the default sign value
	if (type != PrimitiveType::Float) {
		if (sign == 0) {
			name += "unsigned ";
		} else if (sign == 1) {
			name += "signed ";
		}
	}

	TypeScalar* tmp = nullptr;

	switch (type) {
		case PrimitiveType::Byte:
			tmp = new TypeScalar(name + "char", TypeScalar::Int, constness, 8, sign);
			break;
		case PrimitiveType::Short:
			tmp = new TypeScalar(name + "short", TypeScalar::Int, constness, 16, sign);
			break;
		case PrimitiveType::Int:
			tmp = new TypeScalar(name + "int", TypeScalar::Int, constness, 32, sign);
			break;
		case PrimitiveType::Float:
			tmp = new TypeScalar(name + "float", TypeScalar::Float, constness, 32, sign);
			break;
	}

	for (uint64 i = 0; i < types.GetSize(); i++) {
		Type* t = types[i];

		if (t->type != Type::Scalar)
			continue;

		TypeScalar* ts = (TypeScalar*)t;

		if (*ts == tmp) {
			delete tmp;

			return ts;
		}
	}

	types.PushBack(tmp);

	return tmp;
}

TypeVec* Compiler::MakeTypeVec(PrimitiveType type, uint8 constness) {
	String name = constness ? "const " : "";

	TypeVec* tmp = nullptr;

	switch (type) {
		case PrimitiveType::Vec2:
			tmp = new TypeVec(name + "vec2", MakeTypeScalar(PrimitiveType::Float, 0, constness), 2, constness);
			break;
		case PrimitiveType::Vec3:
			tmp = new TypeVec(name + "vec3", MakeTypeScalar(PrimitiveType::Float, 0, constness), 3, constness);
			break;
		case PrimitiveType::Vec4:
			tmp = new TypeVec(name + "vec4", MakeTypeScalar(PrimitiveType::Float, 0, constness), 4, constness);
			break;
	}

	for (uint64 i = 0; i < types.GetSize(); i++) {
		Type* t = types[i];

		if (t->type != Type::Vec)
			continue;

		TypeVec* ts = (TypeVec*)t;

		if (*ts == tmp) {
			delete tmp;

			return ts;
		}
	}

	types.PushBack(tmp);

	return tmp;
}

TypeMat* Compiler::MakeTypeMat(PrimitiveType type, uint8 constness) {
	String name = constness ? "const " : "";

	TypeMat* tmp = nullptr;

	switch (type) {
		case PrimitiveType::Mat4:
			tmp = new TypeMat(name + "mat4", MakeTypeScalar(PrimitiveType::Float, 0, constness), 4, 4, constness);
			break;
	}

	for (uint64 i = 0; i < types.GetSize(); i++) {
		Type* t = types[i];

		if (t->type != Type::Mat)
			continue;

		TypeMat* ts = (TypeMat*)t;

		if (*ts == tmp) {
			delete tmp;

			return ts;
		}
	}

	types.PushBack(tmp);

	return tmp;
}*/



void Compiler::Log(const Token& item, uint64 code, ...) {
	va_list list;
	va_start(list, code);

	switch (code) {
		case HC_ERROR_SYNTAX_MISSING_STRING_CLOSE:
			Log::Error(item.line, item.column, item.filename.str, code, "syntax error: missing closing string character '%c'", va_arg(list, char));
			break;
		case HC_WARN_SYNTAX_INVALID_ESCAPE_CHARACTER:
			Log::Warning(item.line, item.column + va_arg(list, uint64), item.filename.str, code, "syntax error: unrecognized escape character '%c' sequence", va_arg(list, char));
			break;
		case HC_ERROR_SYNTAX_INT_LITERAL_NO_DIGIT:
			Log::Error(item.line, item.column + va_arg(list, uint64), item.filename.str, code, "syntax error: integer literal must have at least one digit");
			break;
		case HC_ERROR_SYNTAX_INT_LITERAL_TO_BIG:
			Log::Error(item.line, item.column + va_arg(list, uint64), item.filename.str, code, "syntax error: integer literal to big for a character '%u'", va_arg(list, uint64));
			break;
		case HC_ERROR_PREPROCESSOR_NO_DIRECTIVE:
			Log::Error(item.line, item.column, item.filename.str, code, "preprocessor error: no directive");
			break;
		case HC_ERROR_PREPROCESSOR_UNKNOWN_DIRECTIVE:
			Log::Error(item.line, item.column, item.filename.str, code, "preprocessor error: unknown preprocessor directive '%s'", va_arg(list, char*));
			break;
		case HC_ERROR_PREPROCESSOR_INCLUDE_FILE_NOT_FOUND:
			Log::Error(item.line, item.column, item.filename.str, code, "preprocessor error: no such file or directory \"%s\"", va_arg(list, char*));
			break;
		case HC_ERROR_PREPROCESSOR_INCLUDE_UNKNOWN_SYMBOL1:
			Log::Error(item.line, item.column, item.filename.str, code, "preprocessor error: unkown symbol in include directive '%c', expected '\"' or '<'", va_arg(list, char));
			break;
		case HC_ERROR_PREPROCESSOR_INCLUDE_UNKNOWN_SYMBOL2:
			Log::Error(item.line, item.column, item.filename.str, code, "preprocessor error: unkown symbol in include directive '%c', expected '%c'", va_arg(list, char), va_arg(list, char));
			break;
		case HC_ERROR_PREPROCESSOR_INCLUDE_RECURSION:
			Log::Error(item.line, item.column, item.filename.str, code, "preprocessor error: '%s' causes recursion", va_arg(list, char*));
			break;
		case HC_WARN_PREPROCESSOR_PRAGMA_UNKNOWN_DIRECTIVE:
			Log::Warning(item.line, item.column, item.filename.str, code, "preprocessor error: unknown pragma directive '%s'", va_arg(list, char*));
			break;
		case HC_WARN_PREPROCESSOR_MACRO_REDEFINITION:
			Log::Warning(item.line, item.column, item.filename.str, code, "preprocessor error: macro redefinition '%s'", va_arg(list, char*));
			break;
		case HC_ERROR_PREPROCESSOR_ERROR_DIRECTIVE:
			Log::Error(item.line, item.column, item.filename.str, code, "preprocessor error: '%s'", va_arg(list, char*));
			break;
		case HC_ERROR_LEXER_EOL:
			Log::Error(item.line, item.column, item.filename.str, code, "lexer error: unexpected end of line");
			break;
		case HC_ERROR_SYNTAX_CHAR_LITERAL_TO_MANY_CHARS:
			Log::Error(item.line, item.column, item.filename.str, code, "syntax error: char literal has to many chars");
			break;
		case HC_ERROR_SYNTAX_EXPECTED:
			Log::Error(item.line, item.column, item.filename.str, code, "syntax error: '%s' expected '%s'", item.string.str, va_arg(list, char*));
			break;
		case HC_ERROR_SYNTAX_ERROR:
			Log::Error(item.line, item.column, item.filename.str, code, "syntax error: '%s'", item.string.str);
			break;
		case HC_ERROR_SYNTAX_ILLEGAL_VARIABLE_NAME:
			Log::Error(item.line, item.column, item.filename.str, code, "syntax error: illegal name '%s', must start with '_', 'a-z' or 'A-Z'. And must not be a keyword or type", item.string.str);
			break;
		case HC_ERROR_SYNTAX_VARIABLE_REDEFINITION:
			Log::Error(item.line, item.column, item.filename.str, code, "syntax error: illegal name '%s', it already exist", item.string.str);
			break;
		case HC_ERROR_SYNTAX_EOL:
			Log::Error(item.line, item.column, item.filename.str, code, "syntax error: unexpected end of file", item.string.str);
			break;
		case HC_ERROR_SYNTAX_INVALID_OPERANDS:
			Log::Error(item.line, item.column, item.filename.str, code, "syntax error: unexpected end of file", item.string.str);
			break;
		case HC_ERROR_SYNTAX_ILLEGAL_TYPENAME:
			Log::Error(item.line, item.column, item.filename.str, code, "syntax error: illegal name '%s', must start with '_', 'a-z' or 'A-Z'. And must not be a keyword or type", item.string.str);
			break;
		case HC_WARN_SEMANTIC_SAME_TYPE_QUALIFIER:
			Log::Warning(item.line, item.column, item.filename.str, code, "semantic error: same type qualifier used more than once");
			break;
		case HC_ERROR_SEMANTIC_SIGNED_UNSIGNED_EXCLUSIVE:
			Log::Error(item.line, item.column, item.filename.str, code, "semantic error: signed/unsigned keywords are mutually exclusive");
			break;
		case HC_ERROR_SEMANTIC_TYPE_FOLLOWED_BY_TYPE:
			Log::Error(item.line, item.column, item.filename.str, code, "semantic error: type '%s' followed by '%s' is illegal", va_arg(list, char*), va_arg(list, char*));
			break;
		case HC_ERROR_SEMANTIC_SIGNED_UNSIGNED_NOT_ALLOWED_ON_TYPE:
			Log::Error(item.line, item.column, item.filename.str, code, "semantic error: '%s' not allowed on type '%s'", va_arg(list, char*), va_arg(list, char*));
			break;
		case HC_WARN_SEMANTIC_PARENT_SCOPE_SYMBOL_REDEFINITION:
			Log::Warning(item.line, item.column, item.filename.str, code, "semantic error: symbol in parent scope overridden");
			break;
		case HC_WARN_SEMANTIC_SYMBOL_REDEFINITION:
			Log::Error(item.line, item.column, item.filename.str, code, "semantic error: symbol '%s' already exist: redefinition", va_arg(list, char*));
			break;
	}
}