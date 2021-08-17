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

#pragma once

#include <core/def.h>

#define HC_ERROR_COMPILER_PREFIX 0x10000
#define HC_ERROR_PREPROCESSOR_PREFIX                        (HC_ERROR_COMPILER_PREFIX | 0x000)
#define HC_ERROR_LEXER_PREFIX                               (HC_ERROR_COMPILER_PREFIX | 0x100)
#define HC_ERROR_SYNTAX_PREFIX                              (HC_ERROR_COMPILER_PREFIX | 0x200)

#define HC_ERROR_PREPROCESSOR(code)                         (HC_ERROR_PREPROCESSOR_PREFIX | (code & 0xFF))
#define HC_ERROR_PREPROCESSOR_NO_DIRECTIVE                  HC_ERROR_PREPROCESSOR(0x01)
#define HC_ERROR_PREPROCESSOR_UNKNOWN_DIRECTIVE             HC_ERROR_PREPROCESSOR(0x02)
#define HC_ERROR_PREPROCESSOR_INCLUDE_FILE_NOT_FOUND        HC_ERROR_PREPROCESSOR(0x03)
#define HC_ERROR_PREPROCESSOR_INCLUDE_UNKNOWN_SYMBOL1       HC_ERROR_PREPROCESSOR(0x04)
#define HC_ERROR_PREPROCESSOR_INCLUDE_UNKNOWN_SYMBOL2       HC_ERROR_PREPROCESSOR(0x05)
#define HC_ERROR_PREPROCESSOR_INCLUDE_RECURSION             HC_ERROR_PREPROCESSOR(0x06)
#define HC_WARN_PREPROCESSOR_PRAGMA_UNKNOWN_DIRECTIVE       HC_ERROR_PREPROCESSOR(0x07)
#define HC_WARN_PREPROCESSOR_MACRO_REDEFINITION             HC_ERROR_PREPROCESSOR(0x08)
#define HC_ERROR_PREPROCESSOR_ERROR_DIRECTIVE               HC_ERROR_PREPROCESSOR(0x09)

#define HC_ERROR_LEXER(code)                                (HC_ERROR_LEXER_PREFIX | (code & 0xFF))
#define HC_ERROR_LEXER_EOL                                  HC_ERROR_LEXER(0x00)

#define HC_ERROR_SYNTAX(code)                               (HC_ERROR_SYNTAX_PREFIX | (code & 0xFF))
#define HC_ERROR_SYNTAX_ERROR                               HC_ERROR_SYNTAX(0x00)
#define HC_ERROR_SYNTAX_MISSING_STRING_CLOSE                HC_ERROR_SYNTAX(0x01)
#define HC_WARN_SYNTAX_INVALID_ESCAPE_CHARACTER             HC_ERROR_SYNTAX(0x02)
#define HC_ERROR_SYNTAX_INT_LITERAL_NO_DIGIT                HC_ERROR_SYNTAX(0x03)
#define HC_ERROR_SYNTAX_INT_LITERAL_TO_BIG                  HC_ERROR_SYNTAX(0x04)
#define HC_ERROR_SYNTAX_CHAR_LITERAL_TO_MANY_CHARS          HC_ERROR_SYNTAX(0x05)
#define HC_WARN_SYNTAX_SAME_TYPE_QUALIFIER                  HC_ERROR_SYNTAX(0x06)
#define HC_ERROR_SYNTAX_SIGNED_UNSIGNED_EXCLUSIVE           HC_ERROR_SYNTAX(0x07)
#define HC_ERROR_SYNTAX_TYPE_FOLLOWED_BY_TYPE               HC_ERROR_SYNTAX(0x08)
#define HC_ERROR_SYNTAX_SIGNED_UNSIGNED_NOT_ALLOWED_ON_TYPE HC_ERROR_SYNTAX(0x09)
#define HC_ERROR_SYNTAX_TYPENAME_ALREADY_EXIST              HC_ERROR_SYNTAX(0x0A)
#define HC_ERROR_SYNTAX_EXPECTED                            HC_ERROR_SYNTAX(0x0B)
#define HC_ERROR_SYNTAX_ILLEGAL_VARIABLE_NAME               HC_ERROR_SYNTAX(0x0C)
#define HC_ERROR_SYNTAX_VARIABLE_REDEFINITION               HC_ERROR_SYNTAX(0x0D)
#define HC_ERROR_SYNTAX_EOL                                 HC_ERROR_SYNTAX(0x0E)
#define HC_ERROR_SYNTAX_INVALID_OPERANDS                    HC_ERROR_SYNTAX(0x0F)
#define HC_ERROR_SYNTAX_ILLEGAL_TYPENAME                    HC_ERROR_SYNTAX(0x10)
