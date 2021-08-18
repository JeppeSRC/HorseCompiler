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

#include "preprocessor.h"

#include <util/file.h>
#include <util/util.h>
#include <core/compiler/compiler.h>

void CorrectIncludeDir(List<String>& includeDir) {
	for (String& string : includeDir) {
		StringUtils::ReplaceChar(string, '\\', '/');

		if (!string.EndsWith("/"))
			string.Append("/");
	}
}

uint64 FindNextNewline(const List<Token>& tokens, uint64 index) {
	uint64        line = tokens[index].line;
	const String& file = tokens[index].filename;

	for (uint64 i = index; i < tokens.GetSize(); i++) {
		const Token& t = tokens[i];

		if (t.line != line || t.filename != file)
			return i - 1;
	}

	return ~0;
}

uint64 FindEndif(const List<Token>& tokens, uint64 index) {
	uint64 count = 0;

	for (uint64 i = index; i < tokens.GetSize(); i++) {
		const Token& t = tokens[i];

		if (t.string[0] != '#')
			continue;

		if (i + 1 == tokens.GetSize()) {
			Compiler::Log(t, HC_ERROR_PREPROCESSOR_NO_DIRECTIVE);
		}

		const Token&  directive = tokens[i + 1];
		const String& str       = directive.string;

		if (str.StartsWith("if")) {
			count++;
		} else if (str == "endif") {
			if (count > 0)
				count--;
			else
				return i;
		}
	}

	return ~0;
}

uint64 FindElse(const List<Token>& tokens, uint64 start, uint64 end) {
	uint64 count = 0;

	for (uint64 i = end - 1; i >= start; i--) {
		const Token& t = tokens[i];

		if (t.string[0] != '#')
			continue;

		const Token&  directive = tokens[i + 1];
		const String& str       = directive.string;

		if (str == "endif") {
			count++;
		} else if (str == "else") {
			if (count > 0)
				count--;
			else
				return i;
		}
	}

	return ~0;
}

List<uint64> FindElifs(const List<Token>& tokens, uint64 start, uint64 end) {
	List<uint64> elifs;

	uint64 count = 0;

	for (uint64 i = start; i < end; i++) {
		const Token& t = tokens[i];

		if (t.string[0] != '#')
			continue;

		const Token&  directive = tokens[i + 1];
		const String& str       = directive.string;

		if (str.StartsWith("if")) {
			count++;
		} else if (str == "endif") {
			count--;
		} else if (str == "elif") {
			if (count == 0)
				elifs.PushBack(i);
		}
	}

	return std::move(elifs);
}

void RemoveComments(List<Token>& tokens) {
	uint64 index = -1;

	while ((index = tokens.Find('/', Token::CharCmp, index + 1)) != ~0) {
		char next = tokens[index + 1].string[0];

		if (next == '/') {
			uint64 newLine = FindNextNewline(tokens, index);

			if (newLine == ~0)
				newLine = tokens.GetSize() - 1;

			tokens.Remove(index, newLine);
		} else if (next == '*') {
			uint64 end = index + 2;

			while ((end = tokens.Find('*', Token::CharCmp, end)) != ~0) {
				if (tokens[end += 1].string[0] == '/')
					break;
			}

			if (end == ~0)
				end = tokens.GetSize() - 1;

			tokens.Remove(index, end);

			if (index == tokens.GetSize()) break;
		}
	}
}

String MergeList(const List<Token>& tokens, uint64 start, uint64 end) {
	String res("");

	int64  currentLine = tokens[start].line;
	String currentFile = tokens[start].filename;

	for (uint64 i = start; i <= end; i++) {
		const Token& t = tokens[i];

		if (currentLine < t.line || currentFile != t.filename) {
			currentLine = t.line;
			currentFile = t.filename;
			res.Append("\n");
		}

		if (t.isString) {
			res.Append(t.string);
		} else {
			res.Append(t.string);

			if (t.trailingSpace)
				res.Append(" ");
		}
	}

	return std::move(res);
}

FileNode* FindCurrentNode(FileNode* nodes, const String& currentFile) {
	if (nodes->name == currentFile)
		return nodes;

	for (uint64 i = 0; i < nodes->files.GetSize(); i++) {
		FileNode* n = FindCurrentNode(nodes->files[i], currentFile);

		if (n)
			return n;
	}

	return nullptr;
}

FileNode* CheckRecursion(FileNode* currentNode, const String& file) {
	if (currentNode->name == file)
		return currentNode;

	if (currentNode->parent) {
		return CheckRecursion(currentNode->parent, file);
	}

	return nullptr;
}

PreProcessor::PreProcessor(List<String>& includeDir, Compiler* compiler) {
	this->includeDir = &includeDir;
	this->compiler   = compiler;
}

bool PreProcessor::Run(List<Token>& tokens) {
	CorrectIncludeDir(*includeDir);
	RemoveComments(tokens);

	FileNode root = { tokens[0].filename, nullptr };

	for (uint64 i = 0; i < tokens.GetSize(); i++) {
		Token& t = tokens[i];

		if (t.isString)
			continue;

		ReplaceDefine(tokens, i);

		char c = t.string[0];

		if (c != '#')
			continue;

		if (i + 1 == tokens.GetSize()) {
			Compiler::Log(t, HC_ERROR_PREPROCESSOR_NO_DIRECTIVE);
		}

		Token& directive = tokens[i + 1];

		if (directive.string == "include") {
			if (!ProcessInclude(tokens, i-- + 2, *includeDir, &root))
				return false;
		} else if (directive.string == "pragma") {
			if (!ProcessPragma(tokens, i-- + 2))
				return false;
		} else if (directive.string == "define") {
			if (!ProcessDefine(tokens, i-- + 2))
				return false;
		} else if (directive.string.StartsWith("if")) {
			if (!ProcessIf(tokens, i-- + 2))
				return false;
		} else if (directive.string.StartsWith("error")) {
			if (!ProcessError(tokens, i-- + 2))
				return false;
		} else {
			Compiler::Log(t, HC_ERROR_PREPROCESSOR_UNKNOWN_DIRECTIVE, directive.string.str);
		}
	}

	return true;
}

bool PreProcessor::ProcessInclude(List<Token>& tokens, uint64 index, const List<String>& includeDir, FileNode* nodes) {
	Token& t = tokens[index];

	bool local     = false;
	char startChar = t.string[0];

	uint64 end     = 0;
	uint64 newLine = FindNextNewline(tokens, index);

	if (t.isString) {
		local = true;
		end   = index;
	} else if (startChar == '<') {
		end = tokens.Find('>', Token::CharCmp, index + 1);
	} else {
		Compiler::Log(t, HC_ERROR_PREPROCESSOR_INCLUDE_UNKNOWN_SYMBOL1, startChar);
		return false;
	}

	if (newLine == ~0)
		newLine = tokens.GetSize() - 1;

	if (end == ~0 || end > newLine) {
		Token& n = tokens[newLine];
		Compiler::Log(n, HC_ERROR_PREPROCESSOR_INCLUDE_UNKNOWN_SYMBOL2, n.string[n.string.length - 1], startChar == '<' ? '>' : '"');
		return false;
	}

	String includeFile = t.string;

	if (!local) {
		MergeList(tokens, index + 1, end - 1);
	}

	String localPath = StringUtils::GetPathFromFilename(t.filename);
	String finalFile;

	if (local) {
		if (FileUtils::FileExist(localPath + includeFile)) {
			finalFile = localPath + includeFile;
		}
	}

	for (uint64 i = 0; i < includeDir.GetSize(); i++) {
		if (finalFile.length > 0)
			break;

		const String& dir = includeDir[i];

		if (FileUtils::FileExist(dir + includeFile)) {
			finalFile = dir + includeFile;
		}
	}

	if (finalFile.length == 0 || finalFile.str == nullptr) {
		Compiler::Log(t, HC_ERROR_PREPROCESSOR_INCLUDE_FILE_NOT_FOUND, includeFile.str);
		return false;
	}

	FileNode* current = FindCurrentNode(nodes, t.filename);
	FileNode* rec     = CheckRecursion(current, finalFile);

	if (rec) {
		Compiler::Log(t, HC_ERROR_PREPROCESSOR_INCLUDE_RECURSION, finalFile.str);
		return false;
	}

	index -= 2;
	tokens.Remove(index, newLine);

	if (includedFiles.Find(finalFile) == ~0) { //Not already included
		FileNode* node = new FileNode;

		node->parent = current;
		node->name   = finalFile;
		current->files.PushBack(node);

		List<Token> res = compiler->LexicalAnalazys(finalFile);
		tokens.Insert(res, index);

	} else {
		Log::Debug("Ignoring \"%s\" already included", finalFile.str);
	}

	return true;
}

bool PreProcessor::ProcessPragma(List<Token>& tokens, uint64 index) {
	Token& pragmaDirective = tokens[index];

	uint64 end = FindNextNewline(tokens, index);

	if (pragmaDirective.string == "once") {
		includedFiles.PushBack(pragmaDirective.filename);
	} else {
		Compiler::Log(pragmaDirective, HC_WARN_PREPROCESSOR_PRAGMA_UNKNOWN_DIRECTIVE, pragmaDirective.string.str);
	}

	tokens.Remove(index - 2, end);

	return true;
}

bool PreProcessor::ProcessDefine(List<Token>& tokens, uint64 index) {
	uint64 newLine = FindNextNewline(tokens, index);

	Token       name = tokens[index];
	List<Token> def;

	for (uint64 i = index + 1; i <= newLine; i++) {
		def.PushBack(tokens[i]);
	}

	uint64 loc = ~0;

	if ((loc = defines.Find(name.string, FindDefineCmp, 0)) != ~0) {
		defines[loc].second = def;
		Compiler::Log(name, HC_WARN_PREPROCESSOR_MACRO_REDEFINITION, name.string.str);
	} else {
		defines.PushBack(std::pair(name.string, def));
	}

	tokens.Remove(index - 2, newLine);

	Log::Debug("Define: %s -> %s", name.string.str, def.GetSize() > 0 ? MergeList(def, 0, def.GetSize() - 1).str : "");

	return true;
}

bool PreProcessor::ProcessIf(List<Token>& tokens, uint64 index) {
	uint64       newLine = FindNextNewline(tokens, index);
	uint64       end     = FindEndif(tokens, newLine);
	uint64       els     = FindElse(tokens, newLine, end);
	List<uint64> elifs   = FindElifs(tokens, newLine, end);

	const String& ifType = tokens[index - 1].string;

	bool res = false;

	if (ifType == "ifdef") {
		res = defines.Find(tokens[index].string, FindDefineCmp, 0) != ~0;
	} else {
		res = EvaluateExpression(tokens, index, newLine) != 0;
	}

	uint64 remStart = ~0;

	if (res) {
		remStart = elifs.GetSize() > 0 ? elifs[0] : els != ~0 ? els
															  : end;
		end += 1;

		tokens.Remove(remStart, end);
		tokens.Remove(index - 2, newLine);
	} else {
		for (uint64 i = 0; i < elifs.GetSize(); i++) {
			uint64 start = elifs[i];
			newLine      = FindNextNewline(tokens, start + 2);
			res          = EvaluateExpression(tokens, start + 2, newLine);

			if (res) {
				remStart = elifs.GetSize() > i + 1 ? elifs[i + 1] : els != ~0 ? els
																			  : end;
				tokens.Remove(remStart, end + 1);
				tokens.Remove(index - 2, newLine);
				break;
			}
		}

		if (remStart == ~0) {
			if (els == ~0) {
				tokens.Remove(index - 2, end + 1);
			} else {
				tokens.Remove(end, end + 1);
				tokens.Remove(index - 2, els + 1);
			}
		}
	}

	return true;
}

bool PreProcessor::ProcessError(List<Token>& tokens, uint64 index) {
	uint64 end = FindNextNewline(tokens, index);

	if (end == ~0) {
		end = tokens.GetSize() - 1;
	}

	String message = MergeList(tokens, index, end);

	Compiler::Log(tokens[index - 1], HC_ERROR_PREPROCESSOR_ERROR_DIRECTIVE, message.str);

	return false;
}

void PreProcessor::ReplaceDefine(List<Token>& tokens, uint64 index) {
	const String& name = tokens[index].string;

	auto [def, items] = defines.FindTuple(name, FindDefineCmp, 0);

	if (def == ~0)
		return;

	tokens.Remove(index, index);
	tokens.Insert(items.second, index);
}

uint64 PreProcessor::EvaluateExpression(List<Token>& tokens, uint64 start, uint64 end) {
	//TODO: implement later
	return false;
}

bool PreProcessor::FindDefineCmp(const std::pair<String, List<Token>>& item, const String& name) {
	return item.first == name;
}