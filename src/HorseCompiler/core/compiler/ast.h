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

#include <util/string.h>
#include <util/list.h>
#include "variable.h"
#include "language.h"

enum class ASTType {
	Unknown,
	Root,
	StatementSequence,
	VariableDeclaration,
	VariableDefinition,
	Parameter,
	FunctionDeclaration,
	FunctionDefinition,
	Body,
	While,
	If,
	For,
	Return,
	Constant,
	Variable,
	Function,
	Operator,
	Assign,
	String,
	Type,
};

/** NodeLayout
* Defines the layout of nodes, what each branch is (if applicable).
*
* Root: Is the root of the ast
*
* StatementSequence: Branches makes up a sequence of statements
*
* VariableDeclaration: Declares a variable
*   Branches:
*       * Type
*       * Name
*
* VariableDefinition: Defines a variable
*   Branches:
*       * Type
*       * Name
*       * (Optional) Expression, to set a default value.
*
* FunctionDeclaration: Declares a function
*   Branches:
*       * Return type
*       * Name
*       * Parameters
*
* FunctionDefinition: Defines a function
*   Branches:
*       * Return type
*       * Name
*       * Parameters
*       * Body
*
* FunctionCall:
*   Branches:
*       * Name
*       * Parameters
*
* Variable: Represents a variable
*   Branches:
*       * Name
*
* Parameter: Represents a parameter
*   Branches:
*       * Type
*       * Name (Optional in declaration)
*
* Operator: Represents a operator
*   Branches:
*       * Operand 0
*       * Operand 1 (depends on operator)
*
* Return: Represents a return statement
*   Branches:
*       * Return value (optional)
*
*/

struct Token;
class ASTNode {
public:
	ASTNode(ASTType type) : parent(nullptr), nodeType(type), token(nullptr) { }
	ASTNode(ASTType type, const Token* token) : parent(nullptr), nodeType(type), token(token) { }

	ASTNode* parent;
	ASTType  nodeType;

	const Token* token;

	List<ASTNode*> branches;

	void AddNode(ASTNode* node) {
		node->parent = this;
		branches.PushBack(node);
	}
};

class StringNode : public ASTNode {
public:
	String string;

	StringNode(const String& string) : ASTNode(ASTType::String), string(string) { }
};

class TypeNode : public ASTNode {
public:
	Type* type;

	TypeNode(Type* type) : ASTNode(ASTType::Type), type(type) { }
};

class ConstantNode : public ASTNode {
public:
	Type* type;

	union {
		uint32 iValue;
		float  fValue;
	};

	ConstantNode(Type* type, uint32 value, const Token* token) : ASTNode(ASTType::Constant, token), type(type), iValue(value) { }
	ConstantNode(Type* type, float value, const Token* token) : ASTNode(ASTType::Constant, token), type(type), fValue(value) { }
};

class OperatorNode : public ASTNode {
public:
	OperatorType type;

	OperatorNode(OperatorType type, const Token* token) : ASTNode(ASTType::Operator, token), type(type) { }
};

/*
class RootNode : public ASTNode {
public:
    RootNode() : ASTNode(ASTType::Root, nullptr) { }
};

class StatementSequencenNode : public ASTNode {
public:
    StatementSequencenNode(ASTNode* parent) : ASTNode(ASTType::StatementSequence, parent) {}

};

class VariableDefinition : public ASTNode {
public:
    Type* type;
    String name;

    VariableDefinition(Variable* variable, ASTNode* parent) : ASTNode(ASTType::VariableDefinition, parent), variable(variable) {}
};*/