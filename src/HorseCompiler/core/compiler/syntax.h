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

#include "token.h"
#include "language.h"
#include "ast.h"

class Syntax {
public:
	static uint64 Analyze(Tokens& lexerResult, uint64 start, ASTNode* currentNode, Language* lang);

private:
	Syntax(Language* lang) : lang(lang) { }

	Language* lang;

	uint64 Analyze(Tokens& lexerResult, uint64 start, ASTNode* currentNode);

	bool            CheckName(const Token& token);
	OperatorTypeDef GetOperator(OperatorType type, OperandType left, OperandType right, bool ignoreOperands);
	OperatorTypeDef GetOperator(List<ASTNode*>& nodes, uint64 index);
	ASTNode*        CreateOperandNode(Tokens& tokens, uint64* index);
	uint64          ParseTypedef(Tokens& tokens, uint64 start, ASTNode* currentNode);
	uint64          ParseTypeDeclaration(Tokens& tokens, uint64 start, TypeNode* typeNode);
	uint64          ParseStruct(Tokens& tokens, uint64 start, ASTNode* currentNode);
	uint64          ParseFunctionParameters(Tokens& tokens, uint64 start, ASTNode* functionNode);
	uint64          ParseExpression(Tokens& tokens, uint64 start, ASTNode* currentNode);
	uint64          ParseLayout(Tokens& tokens, uint64 start, ASTNode* currentNode);
};