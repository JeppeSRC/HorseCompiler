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

#include <core/compiler/compiler.h>
#include <util/string.h>
#include <util/list.h>

struct FileNode {
	String          name; // Name of this file
	FileNode*       parent; // Parent file where it was included
	List<FileNode*> files; // Files included in this file
};

class PreProcessor {
private:
	List<String>*                        includeDir;
	List<String>                         includedFiles; // Files to be ignore if included again
	List<std::pair<String, List<Token>>> defines;
	Compiler*                            compiler;

public:
	PreProcessor(List<String>& includeDir, Compiler* compiler);

	bool Run(List<Token>& result);

private:
	bool ProcessInclude(List<Token>& tokens, uint64 index, const List<String>& includeDir, FileNode* nodes);
	bool ProcessPragma(List<Token>& tokens, uint64 index);
	bool ProcessDefine(List<Token>& tokens, uint64 index);
	bool ProcessIf(List<Token>& tokens, uint64 index);
	bool ProcessError(List<Token>& tokens, uint64 index);

	void   ReplaceDefine(List<Token>& tokens, uint64 index);
	uint64 EvaluateExpression(List<Token>& tokens, uint64 start, uint64 end);

private:
	static bool FindDefineCmp(const std::pair<String, List<Token>>& item, const String& name);
};
