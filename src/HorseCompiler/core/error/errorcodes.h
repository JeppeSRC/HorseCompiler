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
#define HC_ERROR_PREPROCESSOR_PREFIX (HC_ERROR_COMPILER_PREFIX | 0x000)
#define HC_ERROR_SYNTAX_PREFIX (HC_ERROR_COMPILER_PREFIX | 0x100)

#define HC_ERROR_PREPROCESSOR(code) (HC_ERROR_PREPROCESSOR_PREFIX | (code & 0xFF))
#define HC_ERROR_PREPROCESSOR_NO_DIREECTIVE			HC_ERROR_PREPROCESSOR(0x01)

#define HC_ERROR_SYNTAX(code) (HC_ERROR_SYNTAX_PREFIX | (code & 0xFF))
#define HC_ERROR_SYNTAX_MISSING_STRING_CLOSE		HC_ERROR_SYNTAX(0x01)
#define HC_ERROR_SYNTAX_INVALID_ESCAPE_CHARACTER	HC_ERROR_SYNTAX(0x02)
#define HC_ERROR_SYNTAX_INT_LITERAL_NO_DIGIT		HC_ERROR_SYNTAX(0x03)
#define HC_ERROR_SYNTAX_INT_LITERAL_TO_BIG			HC_ERROR_SYNTAX(0x04)
