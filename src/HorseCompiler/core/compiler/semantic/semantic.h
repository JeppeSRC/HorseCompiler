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

#include <core/compiler/lexer/token.h>
#include <core/compiler/language.h>
#include <core/compiler/parsing/ast.h>
#include <core/compiler/misc/symboltable.h>
#include <core/compiler/misc/type.h>

class Semantic {
public:
    static uint64 Analyze(ASTNode* node, TypeTable* typeTable, SymbolTable* symbolTable);

private:
    TypeTable* typeTable;
    SymbolTable* symbolTable;

    Semantic(TypeTable* typeTable, SymbolTable* symbolTable) : typeTable(typeTable), symbolTable(symbolTable) {}

    uint64 Analyze(ASTNode* root);

    uint64 VariableDefinition(ASTNode* node);

    uint64 ConstantEvaluation(ASTNode* node, ASTNode** result);
    uint64 ProcessOperator(OperatorNode* node, ASTNode** result);
    ASTNode* ExecuteOperator(OperatorType op, ASTNode* left, ASTNode* right); //Handles 2 operand operators
    ASTNode* ExecuteOperator(OperatorType op, ASTNode* operand); //Handles single operand operators
};