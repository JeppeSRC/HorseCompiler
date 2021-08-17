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

#include "token.h"
#include "language.h"
#include "ast.h"

class Compiler {
private:
	String    currentDir;
	Language* lang;

	List<Type*> types;

public:
	Compiler(const String& currentDir, Language* lang);

	List<Token> LexicalAnalazys(const String& filename);
	uint64      SyntaxAnalazys(List<Token>& lexerResult, uint64 start, ASTNode* currentNode);

private: // Internal functions
	void ParseLiteral(List<Token>& tokens, uint64 i);
	void ParseStrings(List<Token>& lexerResult);
	void ParseEscapeSequences(Token& token);

	String GetPrimitiveTypeString(const PrimitiveType& type);

	Type*        GetType(const String& name);
	TypeScalar*  MakeTypeScalar(PrimitiveType type, uint8 sign, uint8 constness);
	TypeVec*     MakeTypeVec(PrimitiveType type, uint8 constness);
	TypeMat*     MakeTypeMat(PrimitiveType type, uint8 constness);

	bool            CheckName(const Token& token);
	OperatorTypeDef GetOperator(OperatorType type, OperandType left, OperandType right, bool ignoreOperands);
	OperatorTypeDef GetOperator(List<Token>& tokens, List<ASTNode*>& nodes, uint64 index);
	ASTNode*        CreateOperandNode(List<Token>& tokens, uint64* index);

	uint64 ParseTypedef(List<Token>& tokens, uint64 start, ASTNode* currentNode);
	uint64 ParseTypeDeclaration(List<Token>& tokens, uint64 start, TypeNode* typeNode);
	uint64 ParseStruct(List<Token>& tokens, uint64 start, ASTNode* currentNode);
	uint64 ParseFunctionParameters(List<Token>& tokens, uint64 start, ASTNode* functionNode);
	uint64 ParseExpression(List<Token>& tokens, uint64 start, ASTNode* currentNode);
	uint64 ParseLayout(List<Token>& tokens, uint64 start, ASTNode* currentNode);

	void BacktrackNodes(List<ASTNode*>& nodes);

public: //static stuff
	static void Log(const Token& item, uint64 code, ...);
	static void Log(const ASTNode* item, uint64 code, ...);
};