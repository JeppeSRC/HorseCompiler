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

#pragma once

#include <core/def.h>
#include <core/log/log.h>
#include "errorcodes.h"

#define HC_DEBUG_BREAK __debugbreak()

#ifdef HC_DEBUG
#define HC_ASSERT(x) if (!(x)) { Log::Error("Assertion Failed: %s", #x); HC_DEBUG_BREAK; }
#else
#define HC_ASSERT(x)
#endif
/*
constexpr char* GetCodeString(int64 code) {
	switch (code) {
		case HC_ERROR_SYNTAX_MISSING_STRING_CLOSE:

	}
}*/