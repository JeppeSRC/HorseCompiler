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


#include "log.h"
#include <stdarg.h>
#include <stdio.h>
#include <Windows.h>

#define HC_LOG_COLOR_INFO 0b00001111
#define HC_LOG_COLOR_DEBUG 0b00001010
#define HC_LOG_COLOR_WARNING 0b00001110
#define HC_LOG_COLOR_ERROR 0b00001100

enum class Level {
	Info,
	Debug,
	Warning,
	Error
};

template<Level level>
void LogInternal(const char* const message, va_list args) {

	CONSOLE_SCREEN_BUFFER_INFO info;

	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(handle, &info);

	if constexpr (level == Level::Info) {
		SetConsoleTextAttribute(handle, HC_LOG_COLOR_INFO);
		printf("Info: ");
		vprintf(message, args);
		printf("\n");
	} else if constexpr (level == Level::Debug) {
		SetConsoleTextAttribute(handle, HC_LOG_COLOR_DEBUG);
		printf("Debug: ");
		vprintf(message, args);
		printf("\n");
	} else if constexpr (level == Level::Warning) {
		SetConsoleTextAttribute(handle, HC_LOG_COLOR_WARNING);
		printf("Warning: ");
		vprintf(message, args);
		printf("\n");
	} else if constexpr (level == Level::Error) {
		SetConsoleTextAttribute(handle, HC_LOG_COLOR_ERROR);
		printf("Error: ");
		vprintf(message, args);
		printf("\n");
	}

	SetConsoleTextAttribute(handle, info.wAttributes);
}

template<Level level>
void LogInternal(const char* const filename, int64 line, int64 column, int64 code, const char* const message, va_list args) {

	CONSOLE_SCREEN_BUFFER_INFO info;

	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	GetConsoleScreenBufferInfo(handle, &info);

	if constexpr (level == Level::Info) {
		SetConsoleTextAttribute(handle, HC_LOG_COLOR_INFO);
		printf("%s -> %llu:%llu Info (%llu): ", filename, line, column, code);
		vprintf(message, args);
		printf("\n");
	} else if constexpr (level == Level::Debug) {
		SetConsoleTextAttribute(handle, HC_LOG_COLOR_DEBUG);
		printf("%s -> %llu:%llu Debug (%llu): ", filename, line, column, code);
		vprintf(message, args);
		printf("\n");
	} else if constexpr (level == Level::Warning) {
		SetConsoleTextAttribute(handle, HC_LOG_COLOR_WARNING);
		printf("%s -> %llu:%llu Warning (%llu): ", filename, line, column, code);
		vprintf(message, args);
		printf("\n");
	} else if constexpr (level == Level::Error) {
		SetConsoleTextAttribute(handle, HC_LOG_COLOR_ERROR);
		printf("%s -> %llu:%llu Error (%llu): ", filename, line, column, code);
		vprintf(message, args);
		printf("\n");
	}

	SetConsoleTextAttribute(handle, info.wAttributes);

	if constexpr (level == Level::Error) {
		//TODO: handle errors
	}
}

void Log::Info(const char* const message, ...) { va_list args; va_start(args, message); LogInternal<Level::Info>(message, args); }

void Log::Info(int64 line, int64 column, const char* const filename, int64 code, const char* const message, ...) { va_list args; va_start(args, message); LogInternal<Level::Info>(filename, line, column, code, message, args); }

void Log::Debug(const char* const message, ...) { va_list args; va_start(args, message); LogInternal<Level::Debug>(message, args); }

void Log::Debug(int64 line, int64 column, const char* const filename, int64 code, const char* const message, ...) { va_list args; va_start(args, message); LogInternal<Level::Debug>(filename, line, column, code, message, args); }

void Log::Warning(const char* const message, ...) { va_list args; va_start(args, message); LogInternal<Level::Warning>(message, args); }

void Log::Warning(int64 line, int64 column, const char* const filename, int64 code, const char* const message, ...) { va_list args; va_start(args, message); LogInternal<Level::Warning>(filename, line, column, code, message, args); }

void Log::Error(const char* const message, ...) { va_list args; va_start(args, message); LogInternal<Level::Error>(message, args); }

void Log::Error(int64 line, int64 column, const char* const filename, int64 code, const char* const message, ...) { va_list args; va_start(args, message); LogInternal<Level::Error>(filename, line, column, code, message, args); }
